/*
 *  System.dll
 *
 *  HttpListener: a small HTTP/1.1 server (http only). Each connection has one request and one response, then it is closed
 *  (Connection: close); the body of a request is read all (the limit is 16 MB), the body of a response is kept until Close.
 */

using Collections;
using IO;
using Net.Sockets;
using Text;
using Threading;
using Threading.Tasks;

namespace Net.Http;

////////// ////////// ////////// ////////// //////////

// A request that was received.
public class HttpListenerRequest
{
    private String __method;
    private String __rawUrl;
    private Uri __url;
    private HttpHeaders __headers;
    private UInt8[] __body;
    private IPEndPoint __remote;

    internal HttpListenerRequest(String method, String rawUrl, Uri url, HttpHeaders headers, UInt8[] body, IPEndPoint remote)
    {
        __method = method;
        __rawUrl = rawUrl;
        __url = url;
        __headers = headers;
        __body = body;
        __remote = remote;
    }

    // GET, POST...
    public String HttpMethod => __method;

    // The path and the query as they were sent.
    public String RawUrl => __rawUrl;

    public Uri Url => __url;

    public HttpHeaders Headers => __headers;

    public Boolean HasEntityBody => __body.Length > 0;

    public Int64 ContentLength64 => __body.Length;

    public IPEndPoint RemoteEndPoint => __remote;

    public UInt8[] ReadBody() => __body;

    // The body as UTF-8 text.
    public String ReadBodyAsString() => Encoding.UTF8.GetString(__body);

    public Stream InputStream => new MemoryStream(__body);
}

////////// ////////// ////////// ////////// //////////

// The response to a request: set the status and the headers, write the body to OutputStream (or Write), then Close.
public class HttpListenerResponse
{
    private Socket __socket;
    private HttpHeaders __headers = new HttpHeaders();
    private MemoryStream __body = new MemoryStream();
    private Int32 __status = 200;
    private String? __description;
    private Boolean __sent;
    private Boolean __isHead;
    private UInt8[] __headBytes = new UInt8[0];
    private UInt8[] __bodyBytes = new UInt8[0];

    internal HttpListenerResponse(Socket socket, Boolean isHead)
    {
        __socket = socket;
        __isHead = isHead;
    }

    public Int32 StatusCode
    {
        get { return __status; }
        set { __status = value; }
    }

    public String StatusDescription
    {
        get { return __description == null ? HttpText.Reason(__status) : __description; }
        set { __description = value; }
    }

    public HttpHeaders Headers => __headers;

    public String? ContentType
    {
        get { return __headers.GetValue("Content-Type"); }
        set
        {
            if (value == null)
                __headers.Remove("Content-Type");
            else
                __headers.Set("Content-Type", value);
        }
    }

    // The body: what is written to it is sent when the response is closed.
    public Stream OutputStream => __body;

    public Void Write(UInt8[] bytes)
    {
        __body.Write(bytes, 0, bytes.Length);
    }

    // Writes the text as UTF-8.
    public Void Write(String text)
    {
        Write(Encoding.UTF8.GetBytes(text));
    }

    // Sends the response and closes the connection.
    public Void Close()
    {
        __Prepare();

        try
        {
            __SendAll(__headBytes);
            if (__bodyBytes.Length > 0)
                __SendAll(__bodyBytes);
        }
        finally
        {
            __socket.Close();
        }
    }

    public Task CloseAsync()
    {
        __Prepare();

        try
        {
            NetworkStream stream = new NetworkStream(__socket, false);
            await stream.WriteAsync(__headBytes, 0, __headBytes.Length);

            if (__bodyBytes.Length > 0)
                await stream.WriteAsync(__bodyBytes, 0, __bodyBytes.Length);
        }
        finally
        {
            __socket.Close();
        }
    }

    // Makes the bytes of the head and of the body.
    private Void __Prepare()
    {
        if (__sent)
            throw new InvalidOperationException("the response was sent");

        __sent = true;

        UInt8[] body = __body.ToArray();

        __headers.Set("Content-Length", body.Length.ToString());
        __headers.Set("Connection", "close");

        __headBytes = HttpText.Head("HTTP/1.1 " + __status + " " + StatusDescription, __headers);

        // The answer to HEAD has the length of the body, not the body.
        __bodyBytes = __isHead ? new UInt8[0] : body;
    }

    private Void __SendAll(UInt8[] bytes)
    {
        Int32 offset = 0;
        while (offset < bytes.Length)
            offset += __socket.Send(bytes, offset, bytes.Length - offset);
    }
}

////////// ////////// ////////// ////////// //////////

public class HttpListenerContext
{
    private HttpListenerRequest __request;
    private HttpListenerResponse __response;

    internal HttpListenerContext(HttpListenerRequest request, HttpListenerResponse response)
    {
        __request = request;
        __response = response;
    }

    public HttpListenerRequest Request => __request;

    public HttpListenerResponse Response => __response;
}

////////// ////////// ////////// ////////// //////////

// Waits for the requests of a prefix: "http://host:port/" (the host is an address or a name; "*" and "+" are all the addresses; the
// port is 80 when it is not written, 0 is a port that the system gives: see Port). The path of the prefix is not checked, the
// program tells the paths by Request.Url.
public class HttpListener
{
    private List<String> __prefixes = new List<String>();
    private TcpListener? __listener;
    private Int32 __port;

    // The prefixes, added before Start.
    public List<String> Prefixes => __prefixes;

    // The port that the listener has, after Start.
    public Int32 Port => __port;

    public Boolean IsListening => __listener != null;

    public Void Start()
    {
        if (__listener != null)
            return;

        if (__prefixes.Count == 0)
            throw new InvalidOperationException("the listener has no prefix");

        String prefix = __prefixes[0];
        if (!prefix.StartsWith("http://"))
            throw new ArgumentException("the prefix is not an http address: " + prefix);

        String authority = prefix.Substring(7);
        Int32 slash = authority.IndexOf('/');
        if (slash >= 0)
            authority = authority.Substring(0, slash);

        String host = authority;
        Int32 port = 80;

        Int32 colon = authority.LastIndexOf(':');
        if (colon >= 0)
        {
            host = authority.Substring(0, colon);
            Int64 number = HttpText.FromDecimal(authority.Substring(colon + 1));
            if (number < 0 || number > 65535)
                throw new ArgumentException("the port of the prefix is not valid: " + prefix);

            port = (Int32)number;
        }

        IPAddress address = host == "*" || host == "+" ? IPAddress.Any : Dns.GetHostAddress(host);

        TcpListener listener = new TcpListener(address, port);
        listener.Start();

        __port = listener.LocalEndpoint.Port;
        __listener = listener;
    }

    // Stops the listener: what waits for a request ends with an exception.
    public Void Stop()
    {
        if (__listener != null)
        {
            __listener.Stop();
            __listener = null;
        }
    }

    // Waits for a request (the connection is accepted, the request is read).
    public HttpListenerContext GetContext()
    {
        return GetContextAsync().Result;
    }

    public Task<HttpListenerContext> GetContextAsync()
    {
        return await GetContextAsync(CancellationToken.None);
    }

    public Task<HttpListenerContext> GetContextAsync(CancellationToken cancellationToken)
    {
        TcpListener? listener = __listener;
        if (listener == null)
            throw new InvalidOperationException("the listener is not started");

        for (;;)
        {
            Socket socket = await listener.AcceptSocketAsync(cancellationToken);

            // A connection that sends a request that is not valid is closed, the next one is waited for.
            HttpListenerContext? context = await __Read(socket, cancellationToken);
            if (context != null)
                return context;
        }
    }

    // Reads the request of a connection (null when it is not valid: the connection gets a 400 and is closed).
    private Task<HttpListenerContext?> __Read(Socket socket, CancellationToken token)
    {
        try
        {
            NetworkStream stream = new NetworkStream(socket, false);
            WireReader reader = new WireReader(stream, token);

            String? line = await reader.ReadLine();
            if (line == null)
            {
                socket.Close();
                return null;
            }

            // "GET /path?query HTTP/1.1"
            String[] parts = line.Split(' ');
            if (parts.Length != 3 || !parts[2].StartsWith("HTTP/"))
                throw new HttpRequestException("the request line is not valid: " + line);

            HttpHeaders headers = await reader.ReadHeaders();
            UInt8[] body = await reader.ReadBody(headers, false, 16777216);

            String? host = headers.GetValue("Host");
            Uri url = new Uri("http://" + (host == null ? "localhost" : host) + parts[1]);

            Boolean isHead = parts[0].ToUpper() == "HEAD";

            HttpListenerRequest request = new HttpListenerRequest(parts[0].ToUpper(), parts[1], url, headers, body,
                                                                  socket.RemoteEndPoint);
            return new HttpListenerContext(request, new HttpListenerResponse(socket, isHead));
        }
        catch (OperationCanceledException e)
        {
            socket.Close();
            throw e;
        }
        catch (Exception e)
        {
            __Reject(socket);
            return null;
        }
    }

    // Answers 400 to a connection whose request is not valid, and closes it.
    private static Void __Reject(Socket socket)
    {
        try
        {
            UInt8[] bytes = Encoding.UTF8.GetBytes("HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\nConnection: close\r\n\r\n");
            socket.Send(bytes, 0, bytes.Length);
        }
        catch (Exception e)
        {
        }

        socket.Close();
    }

    // Serves the requests until the token is canceled or the listener is stopped: the handler is called for each one (in its own
    // task: many requests are served at the same time); it writes the response, and Close is called when it ends. An exception of
    // the handler is the answer 500.
    public Task Serve(Delegate<Task, HttpListenerContext> handler, CancellationToken cancellationToken)
    {
        for (;;)
        {
            HttpListenerContext context;

            try
            {
                context = await GetContextAsync(cancellationToken);
            }
            catch (OperationCanceledException e)
            {
                return;
            }
            catch (ObjectDisposedException e)
            {
                return;
            }
            catch (InvalidOperationException e)
            {
                return;
            }

            // The task of the request is not waited for: the loop goes on to the next connection.
            __Handle(handler, context);
        }
    }

    // Runs the handler of a request, then sends the response (a handler that throws is the answer 500).
    private static Task __Handle(Delegate<Task, HttpListenerContext> handler, HttpListenerContext context)
    {
        try
        {
            await handler(context);
        }
        catch (Exception e)
        {
            context.Response.StatusCode = 500;
        }

        try
        {
            await context.Response.CloseAsync();
        }
        catch (Exception e)
        {
        }
    }
}
