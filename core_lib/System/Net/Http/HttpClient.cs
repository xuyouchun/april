/*
 *  System.dll
 *
 *  HttpClient: HTTP/1.1 requests over TCP (http only: there is no TLS).
 */

using IO;
using Net.Sockets;
using Text;
using Threading;
using Threading.Tasks;

namespace Net.Http;

////////// ////////// ////////// ////////// //////////

// Sends requests and gets the responses. A request opens a connection and closes it when the response was read (Connection: close),
// the body of the response is read all (the limit is MaxResponseContentBufferSize). The redirects (301, 302, 303, 307, 308) are
// followed up to MaxAutomaticRedirections times. A request that lasts more than Timeout is canceled (OperationCanceledException).
public class HttpClient : IDisposable
{
    private Uri? __baseAddress;
    private TimeSpan __timeout = new TimeSpan(1000000000);        // 100 seconds (ticks of 100 ns).
    private HttpHeaders __defaultHeaders = new HttpHeaders();
    private Int32 __maxRedirects = 10;
    private Int32 __maxBuffer = 16777216;

    public HttpClient()
    {
    }

    public HttpClient(Uri baseAddress)
    {
        __baseAddress = baseAddress;
    }

    // The address that the relative addresses are resolved against.
    public Uri? BaseAddress
    {
        get { return __baseAddress; }
        set { __baseAddress = value; }
    }

    public TimeSpan Timeout
    {
        get { return __timeout; }
        set { __timeout = value; }
    }

    // The headers that every request has (the ones that the request sets are not replaced).
    public HttpHeaders DefaultRequestHeaders => __defaultHeaders;

    public Int32 MaxAutomaticRedirections
    {
        get { return __maxRedirects; }
        set { __maxRedirects = value; }
    }

    public Int32 MaxResponseContentBufferSize
    {
        get { return __maxBuffer; }
        set { __maxBuffer = value; }
    }

    public Void Dispose()
    {
    }

    private Uri __Resolve(String address)
    {
        if (__baseAddress != null && !address.StartsWith("http://") && !address.StartsWith("https://"))
            return new Uri(__baseAddress, address);

        return new Uri(address);
    }

    public Task<HttpResponseMessage> GetAsync(String address)
    {
        return await SendAsync(new HttpRequestMessage(HttpMethod.Get, __Resolve(address)));
    }

    public Task<HttpResponseMessage> DeleteAsync(String address)
    {
        return await SendAsync(new HttpRequestMessage(HttpMethod.Delete, __Resolve(address)));
    }

    public Task<HttpResponseMessage> PostAsync(String address, HttpContent? content)
    {
        HttpRequestMessage request = new HttpRequestMessage(HttpMethod.Post, __Resolve(address));
        request.Content = content;

        return await SendAsync(request);
    }

    public Task<HttpResponseMessage> PutAsync(String address, HttpContent? content)
    {
        HttpRequestMessage request = new HttpRequestMessage(HttpMethod.Put, __Resolve(address));
        request.Content = content;

        return await SendAsync(request);
    }

    // The body of the response as text; HttpRequestException when the status is not a success.
    public Task<String> GetStringAsync(String address)
    {
        HttpResponseMessage response = await GetAsync(address);
        response.EnsureSuccessStatusCode();

        return await response.Content.ReadAsStringAsync();
    }

    public Task<UInt8[]> GetByteArrayAsync(String address)
    {
        HttpResponseMessage response = await GetAsync(address);
        response.EnsureSuccessStatusCode();

        return await response.Content.ReadAsByteArrayAsync();
    }

    public Task<HttpResponseMessage> SendAsync(HttpRequestMessage request)
    {
        return await SendAsync(request, CancellationToken.None);
    }

    public Task<HttpResponseMessage> SendAsync(HttpRequestMessage request, CancellationToken cancellationToken)
    {
        CancellationTokenSource timeout = new CancellationTokenSource();
        timeout.CancelAfter(__timeout);

        CancellationTokenSource linked = CancellationTokenSource.CreateLinkedTokenSource(cancellationToken, timeout.Token);
        CancellationToken token = linked.Token;

        HttpMethod method = request.Method;
        Uri uri = request.RequestUri;
        HttpContent? content = request.Content;
        HttpResponseMessage response;

        for (Int32 redirects = 0;; redirects++)
        {
            response = await __Exchange(method, uri, request.Headers, content, token);

            Int32 status = response.StatusCode;
            String? location = response.Headers.GetValue("Location");

            Boolean redirect = status == 301 || status == 302 || status == 303 || status == 307 || status == 308;
            if (!redirect || location == null || redirects >= __maxRedirects)
                break;

            uri = new Uri(uri, location);

            // 301, 302 and 303 go on with a GET without a body, 307 and 308 repeat the request.
            if (status == 301 || status == 302 || status == 303)
            {
                method = HttpMethod.Get;
                content = null;
            }
        }

        return response;
    }

    // One request and its response, on a connection of its own.
    private Task<HttpResponseMessage> __Exchange(HttpMethod method, Uri uri, HttpHeaders headers, HttpContent? content,
                                                 CancellationToken token)
    {
        IPAddress address = await Dns.GetHostAddressAsync(uri.Host);
        Socket socket = new Socket();

        try
        {
            socket.NoDelay = true;
            await socket.ConnectAsync(new IPEndPoint(address, uri.Port), token);

            NetworkStream stream = new NetworkStream(socket, false);

            // The head and the body of the request.
            UInt8[] body = content == null ? new UInt8[0] : content.ToArray();

            HttpHeaders sent = new HttpHeaders();
            sent.Add("Host", uri.Port == 80 ? uri.Host : uri.Host + ":" + uri.Port);
            sent.Add("Connection", "close");

            if (content != null || method.Method == "POST" || method.Method == "PUT" || method.Method == "PATCH")
                sent.Add("Content-Length", body.Length.ToString());

            if (content != null)
            {
                for (Int32 k = 0; k < content.Headers.Count; k++)
                    sent.Add(content.Headers.NameAt(k), content.Headers.ValueAt(k));
            }

            for (Int32 k = 0; k < headers.Count; k++)
                sent.Set(headers.NameAt(k), headers.ValueAt(k));

            for (Int32 k = 0; k < __defaultHeaders.Count; k++)
            {
                if (!sent.Contains(__defaultHeaders.NameAt(k)))
                    sent.Add(__defaultHeaders.NameAt(k), __defaultHeaders.ValueAt(k));
            }

            UInt8[] head = HttpText.Head(method.Method + " " + uri.PathAndQuery + " HTTP/1.1", sent);
            await stream.WriteAsync(head, 0, head.Length, token);

            if (body.Length > 0)
                await stream.WriteAsync(body, 0, body.Length, token);

            // The response.
            WireReader reader = new WireReader(stream, token);
            String? status = await reader.ReadLine();
            if (status == null)
                throw new HttpRequestException("the server closed the connection without a response");

            // "HTTP/1.1 200 OK"
            Int32 first = status.IndexOf(' ');
            if (first < 0 || !status.StartsWith("HTTP/"))
                throw new HttpRequestException("the status line is not valid: " + status);

            String rest = status.Substring(first + 1);
            Int32 second = rest.IndexOf(' ');
            String code = second < 0 ? rest : rest.Substring(0, second);

            Int64 number = HttpText.FromDecimal(code);
            if (number < 100 || number > 599)
                throw new HttpRequestException("the status code is not valid: " + status);

            HttpResponseMessage response = new HttpResponseMessage();
            response.StatusCode = (Int32)number;
            response.ReasonPhrase = second < 0 ? "" : rest.Substring(second + 1);

            HttpHeaders received = await reader.ReadHeaders();
            for (Int32 k = 0; k < received.Count; k++)
                response.Headers.Add(received.NameAt(k), received.ValueAt(k));

            // HEAD, 1xx, 204 and 304 have no body.
            Int32 s = response.StatusCode;
            UInt8[] data;

            if (method.Method == "HEAD" || s < 200 || s == 204 || s == 304)
                data = new UInt8[0];
            else
                data = await reader.ReadBody(received, true, __maxBuffer);

            response.Content = new ByteArrayContent(data);
            for (Int32 k = 0; k < received.Count; k++)
            {
                if (received.NameAt(k).ToLower().StartsWith("content-"))
                    response.Content.Headers.Add(received.NameAt(k), received.ValueAt(k));
            }

            return response;
        }
        finally
        {
            socket.Close();
        }
    }
}
