/*
 *  System.dll
 *
 *  StreamReader, StreamWriter: the text of a stream.
 */

using Text;
using Threading.Tasks;

namespace IO;

////////// ////////// ////////// ////////// //////////

// Reads the text of a stream. The bytes that are left in the stream are read and decoded when the first read is made (it keeps
// the code simple: a character made of many bytes is never cut), then the text is given line by line. A Unicode mark at the
// beginning is not part of the text.
public class StreamReader : IDisposable
{
    private Stream __stream;
    private Encoding __encoding;
    private String? __text;
    private Int32 __position;

    public StreamReader(Stream stream) : this(stream, Encoding.UTF8)
    {
    }

    public StreamReader(Stream stream, Encoding encoding)
    {
        __stream = stream;
        __encoding = encoding;
    }

    public StreamReader(String path) : this(new FileStream(path, FileMode.Open, FileAccess.Read), Encoding.UTF8)
    {
    }

    // Whether all the text was read.
    public Boolean EndOfStream
    {
        get
        {
            __Load();
            return __position >= __text.Length;
        }
    }

    // The next character, -1 at the end.
    public Int32 Read()
    {
        __Load();
        return __Next();
    }

    // The next line without its end (\n or \r\n), null at the end.
    public String? ReadLine()
    {
        __Load();
        return __Line();
    }

    // All the text that is left.
    public String ReadToEnd()
    {
        __Load();
        return __Rest();
    }

    public Task<String?> ReadLineAsync()
    {
        await __LoadAsync();
        return __Line();
    }

    public Task<String> ReadToEndAsync()
    {
        await __LoadAsync();
        return __Rest();
    }

    public Void Close()
    {
        __stream.Close();
    }

    public Void Dispose()
    {
        Close();
    }

    private Int32 __Next()
    {
        if (__position >= __text.Length)
            return -1;

        Int32 c = (Int32)__text[__position];
        __position = __position + 1;

        return c;
    }

    private String? __Line()
    {
        String text = __text;
        if (__position >= text.Length)
            return null;

        Int32 end = __position;
        while (end < text.Length && text[end] != '\n' && text[end] != '\r')
            end = end + 1;

        String line = text.Substring(__position, end - __position);

        if (end < text.Length && text[end] == '\r' && end + 1 < text.Length && text[end + 1] == '\n')
            end = end + 1;

        __position = end < text.Length ? end + 1 : end;
        return line;
    }

    private String __Rest()
    {
        String rest = __text.Substring(__position, __text.Length - __position);
        __position = __text.Length;

        return rest;
    }

    private Void __Load()
    {
        if (__text != null)
            return;

        MemoryStream bytes = new MemoryStream();
        __stream.CopyTo(bytes);

        __Decode(bytes.ToArray());
    }

    private Task __LoadAsync()
    {
        if (__text != null)
            return;

        MemoryStream bytes = new MemoryStream();
        await __stream.CopyToAsync(bytes);

        __Decode(bytes.ToArray());
    }

    private Void __Decode(UInt8[] data)
    {
        Int32 start = 0;

        // The mark of UTF-8 (EF BB BF).
        if (data.Length >= 3 && data[0] == 239 && data[1] == 187 && data[2] == 191)
            start = 3;

        __text = __encoding.GetString(data, start, data.Length - start);
        __position = 0;
    }
}

////////// ////////// ////////// ////////// //////////

// Writes text to a stream: the text is kept until Flush (or Close), when it is encoded and written.
public class StreamWriter : IDisposable
{
    private Stream __stream;
    private Encoding __encoding;
    private StringBuilder __buffer = new StringBuilder();
    private Boolean __autoFlush;

    public StreamWriter(Stream stream) : this(stream, Encoding.UTF8)
    {
    }

    public StreamWriter(Stream stream, Encoding encoding)
    {
        __stream = stream;
        __encoding = encoding;
    }

    public StreamWriter(String path) : this(new FileStream(path, FileMode.Create, FileAccess.Write), Encoding.UTF8)
    {
    }

    public StreamWriter(String path, Boolean append)
        : this(new FileStream(path, append ? FileMode.Append : FileMode.Create, FileAccess.Write), Encoding.UTF8)
    {
    }

    // Flush after each Write.
    public Boolean AutoFlush
    {
        get { return __autoFlush; }
        set { __autoFlush = value; }
    }

    public Void Write(String? value)
    {
        if (value != null)
            __buffer.Append(value);

        if (__autoFlush)
            Flush();
    }

    public Void Write(Char value) => Write(value.ToString());

    public Void Write(Int32 value) => Write(value.ToString());

    public Void Write(Int64 value) => Write(value.ToString());

    public Void Write(Double value) => Write(value.ToString());

    public Void Write(Boolean value) => Write(value.ToString());

    public Void WriteLine()
    {
        Write("\n");
    }

    public Void WriteLine(String? value)
    {
        __buffer.Append(value == null ? "" : value);
        __buffer.Append("\n");

        if (__autoFlush)
            Flush();
    }

    // Encodes the text that was written and writes it to the stream.
    public Void Flush()
    {
        if (__buffer.Length > 0)
        {
            UInt8[] bytes = __encoding.GetBytes(__buffer.ToString());
            __buffer.Clear();

            __stream.Write(bytes, 0, bytes.Length);
        }

        __stream.Flush();
    }

    public Task WriteAsync(String? value)
    {
        if (value != null)
            __buffer.Append(value);

        await FlushAsync();
    }

    public Task WriteLineAsync(String? value)
    {
        __buffer.Append(value == null ? "" : value);
        __buffer.Append("\n");

        await FlushAsync();
    }

    public Task FlushAsync()
    {
        if (__buffer.Length > 0)
        {
            UInt8[] bytes = __encoding.GetBytes(__buffer.ToString());
            __buffer.Clear();

            await __stream.WriteAsync(bytes, 0, bytes.Length);
        }

        await __stream.FlushAsync();
    }

    public Void Close()
    {
        Flush();
        __stream.Close();
    }

    public Void Dispose()
    {
        Close();
    }
}
