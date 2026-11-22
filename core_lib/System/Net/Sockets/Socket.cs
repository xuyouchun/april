/*
 *  System.dll
 *
 *  Socket: a TCP socket (IPv4). The methods that wait have two forms: the ones that block the thread, and the ones with Async in the
 *  name that give a task: those do not hold a thread while they wait (the reactor waits for all the sockets, see SocketReactor).
 */

using Threading;
using Threading.Tasks;

namespace Net.Sockets;

////////// ////////// ////////// ////////// //////////

public enum SocketShutdown
{
    Receive = 0,
    Send = 1,
    Both = 2,
}

////////// ////////// ////////// ////////// //////////

public class Socket : IDisposable
{
    private Int64 __handle;
    private Boolean __closed;

    // A new socket that is not connected.
    public Socket()
    {
        Int64 handle = Internal.Socket_Create();
        SocketError.Check(handle, "socket");

        __handle = handle;
    }

    // The socket of a connection that was accepted.
    internal Socket(Int64 handle)
    {
        __handle = handle;
    }

    public Boolean IsClosed => __closed;

    // Whether the data is sent at once (not kept to be sent with the next).
    public Boolean NoDelay
    {
        set
        {
            __Check();
            SocketError.Check(Internal.Socket_SetOption(__handle, 1, value ? 1 : 0), "option");
        }
    }

    public Boolean KeepAlive
    {
        set
        {
            __Check();
            SocketError.Check(Internal.Socket_SetOption(__handle, 3, value ? 1 : 0), "option");
        }
    }

    // Lets the address be used again at once after the socket is closed.
    public Boolean ReuseAddress
    {
        set
        {
            __Check();
            SocketError.Check(Internal.Socket_SetOption(__handle, 2, value ? 1 : 0), "option");
        }
    }

    public IPEndPoint LocalEndPoint
    {
        get
        {
            __Check();

            Int32 port = Internal.Socket_LocalPort(__handle);
            SocketError.Check(port, "local address");

            return new IPEndPoint(IPAddress.Any, port);
        }
    }

    public IPEndPoint RemoteEndPoint
    {
        get
        {
            __Check();

            Int32 port = Internal.Socket_RemotePort(__handle);
            SocketError.Check(port, "remote address");

            String? text = Internal.Socket_RemoteAddress(__handle);
            return new IPEndPoint(text == null ? IPAddress.Any : IPAddress.Parse(text), port);
        }
    }

    public Void Bind(IPEndPoint endPoint)
    {
        __Check();
        SocketError.Check(Internal.Socket_Bind(__handle, endPoint.Address.ToString(), endPoint.Port), "bind");
    }

    public Void Listen(Int32 backlog)
    {
        __Check();
        SocketError.Check(Internal.Socket_Listen(__handle, backlog), "listen");
    }

    // Waits for a connection, blocks the thread.
    public Socket Accept()
    {
        for (;;)
        {
            __Check();

            Int64 handle = Internal.Socket_Accept(__handle);
            if (handle != -1)
            {
                SocketError.Check(handle, "accept");
                return new Socket(handle);
            }

            __WaitHere(1);
        }
    }

    public Task<Socket> AcceptAsync()
    {
        return AcceptAsync(CancellationToken.None);
    }

    public Task<Socket> AcceptAsync(CancellationToken cancellationToken)
    {
        for (;;)
        {
            __Check();

            Int64 handle = Internal.Socket_Accept(__handle);
            if (handle != -1)
            {
                SocketError.Check(handle, "accept");
                return new Socket(handle);
            }

            await SocketReactor.Wait(__handle, 1, cancellationToken);
        }
    }

    public Void Connect(IPEndPoint endPoint)
    {
        __Check();

        Int32 r = Internal.Socket_Connect(__handle, endPoint.Address.ToString(), endPoint.Port);
        if (r == -1)
        {
            __WaitHere(2);
            r = Internal.Socket_FinishConnect(__handle);
        }

        SocketError.Check(r, "connect to " + endPoint.ToString());
    }

    public Void Connect(String host, Int32 port)
    {
        Connect(new IPEndPoint(Dns.GetHostAddress(host), port));
    }

    public Task ConnectAsync(IPEndPoint endPoint)
    {
        return ConnectAsync(endPoint, CancellationToken.None);
    }

    public Task ConnectAsync(IPEndPoint endPoint, CancellationToken cancellationToken)
    {
        __Check();

        Int32 r = Internal.Socket_Connect(__handle, endPoint.Address.ToString(), endPoint.Port);
        if (r == -1)
        {
            await SocketReactor.Wait(__handle, 2, cancellationToken);
            r = Internal.Socket_FinishConnect(__handle);
        }

        SocketError.Check(r, "connect to " + endPoint.ToString());
    }

    public Task ConnectAsync(String host, Int32 port)
    {
        IPAddress address = await Dns.GetHostAddressAsync(host);
        await ConnectAsync(new IPEndPoint(address, port), CancellationToken.None);
    }

    // Sends bytes: returns how many were sent (it can be less than count).
    public Int32 Send(UInt8[] buffer, Int32 offset, Int32 count)
    {
        __CheckRange(buffer, offset, count);

        for (;;)
        {
            __Check();

            Int32 n = count == 0 ? 0 : Internal.Socket_Send(__handle, Ptr.AddressOf(buffer, offset), count);
            if (n != -1)
            {
                SocketError.Check(n, "send");
                return n;
            }

            __WaitHere(2);
        }
    }

    public Task<Int32> SendAsync(UInt8[] buffer, Int32 offset, Int32 count)
    {
        return SendAsync(buffer, offset, count, CancellationToken.None);
    }

    public Task<Int32> SendAsync(UInt8[] buffer, Int32 offset, Int32 count, CancellationToken cancellationToken)
    {
        __CheckRange(buffer, offset, count);

        for (;;)
        {
            __Check();

            Int32 n = count == 0 ? 0 : Internal.Socket_Send(__handle, Ptr.AddressOf(buffer, offset), count);
            if (n != -1)
            {
                SocketError.Check(n, "send");
                return n;
            }

            await SocketReactor.Wait(__handle, 2, cancellationToken);
        }
    }

    // Receives bytes: returns how many (0: the other side closed the connection), blocks the thread until there are some.
    public Int32 Receive(UInt8[] buffer, Int32 offset, Int32 count)
    {
        __CheckRange(buffer, offset, count);

        for (;;)
        {
            __Check();

            Int32 n = count == 0 ? 0 : Internal.Socket_Receive(__handle, Ptr.AddressOf(buffer, offset), count);
            if (n != -1)
            {
                SocketError.Check(n, "receive");
                return n;
            }

            __WaitHere(1);
        }
    }

    public Task<Int32> ReceiveAsync(UInt8[] buffer, Int32 offset, Int32 count)
    {
        return ReceiveAsync(buffer, offset, count, CancellationToken.None);
    }

    public Task<Int32> ReceiveAsync(UInt8[] buffer, Int32 offset, Int32 count, CancellationToken cancellationToken)
    {
        __CheckRange(buffer, offset, count);

        for (;;)
        {
            __Check();

            Int32 n = count == 0 ? 0 : Internal.Socket_Receive(__handle, Ptr.AddressOf(buffer, offset), count);
            if (n != -1)
            {
                SocketError.Check(n, "receive");
                return n;
            }

            await SocketReactor.Wait(__handle, 1, cancellationToken);
        }
    }

    public Void Shutdown(SocketShutdown how)
    {
        __Check();
        Internal.Socket_Shutdown(__handle, (Int32)how);
    }

    // Closes the socket: what waits for it ends with an exception.
    public Void Close()
    {
        if (__closed)
            return;

        __closed = true;
        Internal.Socket_Close(__handle);
        SocketReactor.Release(__handle);
    }

    public Void Dispose()
    {
        Close();
    }

    private Void __Check()
    {
        if (__closed)
            throw new ObjectDisposedException("the socket is closed");
    }

    private Void __CheckRange(UInt8[] buffer, Int32 offset, Int32 count)
    {
        if (buffer == null)
            throw new ArgumentException("the buffer is null");

        if (offset < 0 || count < 0 || offset > buffer.Length - count)
            throw new OutOfRangeException();
    }

    // Waits in this thread until the socket is ready (a closed socket is ready too: the next call tells the error).
    private Void __WaitHere(Int32 events)
    {
        Int64[] handles = new Int64[1];
        Int32[] wanted = new Int32[1];
        Int32[] results = new Int32[1];

        handles[0] = __handle;
        wanted[0] = events;

        Int32 ready = Internal.Socket_Poll(Ptr.AddressOf(handles), Ptr.AddressOf(wanted), Ptr.AddressOf(results), 1, -1);
        if (ready < 0)
            Thread.Sleep(1);
    }
}
