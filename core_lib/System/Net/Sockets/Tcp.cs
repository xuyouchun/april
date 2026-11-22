/*
 *  System.dll
 *
 *  NetworkStream, TcpListener, TcpClient.
 */

using IO;
using Threading;
using Threading.Tasks;

namespace Net.Sockets;

////////// ////////// ////////// ////////// //////////

// The bytes of a connected socket as a stream: it cannot seek. Write sends all the bytes. Reading 0 bytes means that the other side
// closed the connection.
public class NetworkStream : Stream
{
    private Socket __socket;
    private Boolean __ownsSocket;

    public NetworkStream(Socket socket) : this(socket, false)
    {
    }

    public NetworkStream(Socket socket, Boolean ownsSocket)
    {
        __socket = socket;
        __ownsSocket = ownsSocket;
    }

    public override Boolean CanRead => !__socket.IsClosed;

    public override Boolean CanWrite => !__socket.IsClosed;

    public override Boolean CanSeek => false;

    public override Int64 Length
    {
        get { throw new NotSupportedException("a network stream has no length"); }
    }

    public override Int64 Position
    {
        get { throw new NotSupportedException("a network stream has no position"); }
        set { throw new NotSupportedException("a network stream has no position"); }
    }

    public override Int32 Read(UInt8[] buffer, Int32 offset, Int32 count)
    {
        return __socket.Receive(buffer, offset, count);
    }

    public override Void Write(UInt8[] buffer, Int32 offset, Int32 count)
    {
        while (count > 0)
        {
            Int32 n = __socket.Send(buffer, offset, count);
            offset = offset + n;
            count = count - n;
        }
    }

    public override Task<Int32> ReadAsync(UInt8[] buffer, Int32 offset, Int32 count, CancellationToken cancellationToken)
    {
        return await __socket.ReceiveAsync(buffer, offset, count, cancellationToken);
    }

    public override Task WriteAsync(UInt8[] buffer, Int32 offset, Int32 count, CancellationToken cancellationToken)
    {
        while (count > 0)
        {
            Int32 n = await __socket.SendAsync(buffer, offset, count, cancellationToken);
            offset = offset + n;
            count = count - n;
        }
    }

    public override Void Flush()
    {
    }

    public override Int64 Seek(Int64 offset, SeekOrigin origin)
    {
        throw new NotSupportedException("a network stream cannot seek");
    }

    public override Void SetLength(Int64 value)
    {
        throw new NotSupportedException("a network stream has no length");
    }

    public override Void Close()
    {
        if (__ownsSocket)
            __socket.Close();
    }
}

////////// ////////// ////////// ////////// //////////

// Waits for the connections to an address.
public class TcpListener
{
    private IPEndPoint __endPoint;
    private Socket? __socket;

    public TcpListener(IPAddress address, Int32 port)
    {
        __endPoint = new IPEndPoint(address, port);
    }

    // The address that the listener has (the port is the one that was given by the system when 0 was asked).
    public IPEndPoint LocalEndpoint
    {
        get
        {
            if (__socket == null)
                return __endPoint;

            return new IPEndPoint(__endPoint.Address, __socket.LocalEndPoint.Port);
        }
    }

    public Void Start()
    {
        Start(128);
    }

    public Void Start(Int32 backlog)
    {
        if (__socket != null)
            return;

        Socket socket = new Socket();

        try
        {
            socket.ReuseAddress = true;
            socket.Bind(__endPoint);
            socket.Listen(backlog);
        }
        catch (Exception e)
        {
            socket.Close();
            throw e;
        }

        __socket = socket;
    }

    // Stops the listener: what waits for a connection ends with an exception.
    public Void Stop()
    {
        if (__socket != null)
        {
            __socket.Close();
            __socket = null;
        }
    }

    public Socket AcceptSocket()
    {
        return __Started().Accept();
    }

    public Task<Socket> AcceptSocketAsync()
    {
        return await __Started().AcceptAsync();
    }

    public Task<Socket> AcceptSocketAsync(CancellationToken cancellationToken)
    {
        return await __Started().AcceptAsync(cancellationToken);
    }

    public TcpClient AcceptTcpClient()
    {
        return new TcpClient(AcceptSocket());
    }

    public Task<TcpClient> AcceptTcpClientAsync()
    {
        Socket socket = await AcceptSocketAsync();
        return new TcpClient(socket);
    }

    public Task<TcpClient> AcceptTcpClientAsync(CancellationToken cancellationToken)
    {
        Socket socket = await AcceptSocketAsync(cancellationToken);
        return new TcpClient(socket);
    }

    private Socket __Started()
    {
        Socket? socket = __socket;
        if (socket == null)
            throw new InvalidOperationException("the listener is not started");

        return socket;
    }
}

////////// ////////// ////////// ////////// //////////

// A connection to a server (or one that a listener accepted).
public class TcpClient : IDisposable
{
    private Socket __socket;
    private Boolean __connected;

    public TcpClient()
    {
        __socket = new Socket();
    }

    // Connects to the host (a name or an address) and the port.
    public TcpClient(String host, Int32 port)
    {
        __socket = new Socket();
        Connect(host, port);
    }

    internal TcpClient(Socket accepted)
    {
        __socket = accepted;
        __connected = true;
    }

    public Socket Client => __socket;

    public Boolean Connected => __connected && !__socket.IsClosed;

    public Boolean NoDelay
    {
        set { __socket.NoDelay = value; }
    }

    public Void Connect(String host, Int32 port)
    {
        __socket.Connect(host, port);
        __connected = true;
    }

    public Task ConnectAsync(String host, Int32 port)
    {
        await __socket.ConnectAsync(host, port);
        __connected = true;
    }

    public NetworkStream GetStream()
    {
        if (!__connected)
            throw new InvalidOperationException("the client is not connected");

        return new NetworkStream(__socket, false);
    }

    public Void Close()
    {
        __socket.Close();
    }

    public Void Dispose()
    {
        Close();
    }
}
