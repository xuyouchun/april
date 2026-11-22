/*
 *  System.dll
 *
 *  The HTTP/1.1 text on the wire, shared by HttpClient and HttpListener: a reader of lines and bodies, and the writing of the heads.
 */

using Collections;
using IO;
using Text;
using Threading;
using Threading.Tasks;

namespace Net.Http;

////////// ////////// ////////// ////////// //////////

// Reads a stream by lines and by blocks. The heads are limited (64 KB) so that a peer cannot make the memory grow.
internal class WireReader
{
    private Stream __stream;
    private UInt8[] __buffer = new UInt8[4096];
    private Int32 __start;
    private Int32 __end;
    private CancellationToken __token;

    internal WireReader(Stream stream, CancellationToken token)
    {
        __stream = stream;
        __token = token;
    }

    // Reads more bytes into the buffer: false at the end of the stream.
    private Task<Boolean> __Fill()
    {
        if (__start < __end)
            return true;

        __start = 0;
        __end = await __stream.ReadAsync(__buffer, 0, __buffer.Length, __token);

        return __end > 0;
    }

    // A line without its end (CRLF or LF): null at the end of the stream before any byte.
    internal Task<String?> ReadLine()
    {
        List<UInt8> bytes = new List<UInt8>();
        Boolean any = false;

        for (;;)
        {
            Boolean more = await __Fill();
            if (!more)
                break;

            any = true;
            UInt8 b = __buffer[__start];
            __start = __start + 1;

            if (b == 10)
                break;

            bytes.Add(b);
            if (bytes.Count > 65536)
                throw new HttpRequestException("a line of the message is too long");
        }

        if (!any)
            return null;

        if (bytes.Count > 0 && bytes[bytes.Count - 1] == 13)
            bytes.RemoveAt(bytes.Count - 1);

        return Encoding.UTF8.GetString(bytes.ToArray());
    }

    // Exactly count bytes (fewer when the stream ends).
    internal Task<UInt8[]> ReadBytes(Int32 count)
    {
        UInt8[] result = new UInt8[count];
        Int32 total = 0;

        while (total < count)
        {
            Boolean more = await __Fill();
            if (!more)
                break;

            Int32 n = __end - __start;
            if (n > count - total)
                n = count - total;

            Bytes.Copy(__buffer, __start, result, total, n);
            __start = __start + n;
            total = total + n;
        }

        if (total == count)
            return result;

        UInt8[] shorter = new UInt8[total];
        Bytes.Copy(result, 0, shorter, 0, total);

        return shorter;
    }

    // All that is left until the stream ends (the length is limited by max).
    internal Task<UInt8[]> ReadToEnd(Int32 max)
    {
        MemoryStream all = new MemoryStream();

        for (;;)
        {
            Boolean more = await __Fill();
            if (!more)
                break;

            all.Write(__buffer, __start, __end - __start);
            __start = __end;

            if (all.Length > max)
                throw new HttpRequestException("the body is too big");
        }

        return all.ToArray();
    }

    // The body in chunks: size in hex, CRLF, the bytes, CRLF... and a size 0, then the trailers.
    internal Task<UInt8[]> ReadChunked(Int32 max)
    {
        MemoryStream all = new MemoryStream();

        for (;;)
        {
            String? line = await ReadLine();
            if (line == null)
                throw new HttpRequestException("the chunked body ended too soon");

            Int32 semicolon = line.IndexOf(';');
            Int32 size = HttpText.FromHex(semicolon < 0 ? line.Trim() : line.Substring(0, semicolon).Trim());
            if (size < 0)
                throw new HttpRequestException("the size of a chunk is not valid");

            if (size == 0)
                break;

            UInt8[] chunk = await ReadBytes(size);
            if (chunk.Length != size)
                throw new HttpRequestException("the chunked body ended too soon");

            all.Write(chunk, 0, chunk.Length);
            if (all.Length > max)
                throw new HttpRequestException("the body is too big");

            // The CRLF after the bytes.
            await ReadLine();
        }

        // The trailers, until the empty line.
        for (;;)
        {
            String? trailer = await ReadLine();
            if (trailer == null || trailer.Length == 0)
                break;
        }

        return all.ToArray();
    }

    // The headers until the empty line.
    internal Task<HttpHeaders> ReadHeaders()
    {
        HttpHeaders headers = new HttpHeaders();
        Int32 size = 0;

        for (;;)
        {
            String? line = await ReadLine();
            if (line == null || line.Length == 0)
                break;

            size += line.Length;
            if (size > 65536)
                throw new HttpRequestException("the headers are too big");

            Int32 colon = line.IndexOf(':');
            if (colon <= 0)
                throw new HttpRequestException("a header is not valid: " + line);

            headers.Add(line.Substring(0, colon).Trim(), line.Substring(colon + 1).Trim());
        }

        return headers;
    }

    // The body of a message by its headers: Content-Length, chunked, or until the stream ends (readToEndIfNoLength).
    internal Task<UInt8[]> ReadBody(HttpHeaders headers, Boolean readToEndIfNoLength, Int32 max)
    {
        String? encoding = headers.GetJoined("Transfer-Encoding");
        if (encoding != null && encoding.ToLower().Contains("chunked"))
            return await ReadChunked(max);

        String? length = headers.GetValue("Content-Length");
        if (length != null)
        {
            Int64 n = HttpText.FromDecimal(length.Trim());
            if (n < 0 || n > max)
                throw new HttpRequestException("the length of the body is not valid or too big");

            return await ReadBytes((Int32)n);
        }

        if (readToEndIfNoLength)
            return await ReadToEnd(max);

        return new UInt8[0];
    }
}

////////// ////////// ////////// ////////// //////////

internal static class HttpText
{
    // The number of a text of digits, -1 if it is not one.
    internal static Int64 FromDecimal(String text)
    {
        if (text.Length == 0 || text.Length > 18)
            return -1;

        Int64 value = 0;
        for (Int32 k = 0; k < text.Length; k++)
        {
            Int32 d = (Int32)text[k] - 48;
            if (d < 0 || d > 9)
                return -1;

            value = value * 10 + d;
        }

        return value;
    }

    internal static Int32 FromHex(String text)
    {
        if (text.Length == 0 || text.Length > 7)
            return -1;

        Int32 value = 0;
        for (Int32 k = 0; k < text.Length; k++)
        {
            Int32 c = (Int32)text[k];
            Int32 d;

            if (c >= 48 && c <= 57)
                d = c - 48;
            else if (c >= 97 && c <= 102)
                d = c - 87;
            else if (c >= 65 && c <= 70)
                d = c - 55;
            else
                return -1;

            value = value * 16 + d;
        }

        return value;
    }

    // The reason phrase of a status code.
    internal static String Reason(Int32 status)
    {
        switch (status)
        {
            case 200: return "OK";
            case 201: return "Created";
            case 202: return "Accepted";
            case 204: return "No Content";
            case 301: return "Moved Permanently";
            case 302: return "Found";
            case 303: return "See Other";
            case 304: return "Not Modified";
            case 307: return "Temporary Redirect";
            case 308: return "Permanent Redirect";
            case 400: return "Bad Request";
            case 401: return "Unauthorized";
            case 403: return "Forbidden";
            case 404: return "Not Found";
            case 405: return "Method Not Allowed";
            case 408: return "Request Timeout";
            case 409: return "Conflict";
            case 411: return "Length Required";
            case 413: return "Payload Too Large";
            case 500: return "Internal Server Error";
            case 501: return "Not Implemented";
            case 502: return "Bad Gateway";
            case 503: return "Service Unavailable";
            default: return "";
        }
    }

    // The bytes of a head: the first line, the headers (not the ones that are in skip), and the empty line.
    internal static UInt8[] Head(String firstLine, HttpHeaders headers)
    {
        StringBuilder text = new StringBuilder();
        text.Append(firstLine);
        text.Append("\r\n");

        for (Int32 k = 0; k < headers.Count; k++)
        {
            text.Append(headers.NameAt(k));
            text.Append(": ");
            text.Append(headers.ValueAt(k));
            text.Append("\r\n");
        }

        text.Append("\r\n");
        return Encoding.UTF8.GetBytes(text.ToString());
    }
}
