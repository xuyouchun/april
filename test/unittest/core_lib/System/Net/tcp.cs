// TCP over the loopback: a server and clients in the same program. The async methods do not hold a thread while they wait (the reactor
// waits for the sockets): 200 connections are open at the same time (the pool has 64 threads at most).

/* EXPECT

address 127.0.0.1:80 True False
parse False False True
sync echo hello
async echo ping 1
async echo ping 2
many 200 20100
refused SocketException
closed 0
done

*/

import System;

using System;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

class Program
{
    // Echoes what a connection sends, until it closes.
    static Task Serve(TcpClient client)
    {
        NetworkStream stream = client.GetStream();
        UInt8[] buffer = new UInt8[256];

        for (;;)
        {
            Int32 n = await stream.ReadAsync(buffer, 0, buffer.Length);
            if (n == 0)
                break;

            await stream.WriteAsync(buffer, 0, n);
        }

        client.Close();
    }

    static Task Listen(TcpListener listener, Int32 count)
    {
        Task[] served = new Task[count];

        for (Int32 k = 0; k < count; k++)
        {
            TcpClient client = await listener.AcceptTcpClientAsync();
            served[k] = Serve(client);
        }

        for (Int32 k = 0; k < count; k++)
            await served[k];
    }

    static Task<String> Ask(Int32 port, String text)
    {
        TcpClient client = new TcpClient();
        await client.ConnectAsync("127.0.0.1", port);

        NetworkStream stream = client.GetStream();
        UInt8[] bytes = Encoding.UTF8.GetBytes(text);
        await stream.WriteAsync(bytes, 0, bytes.Length);

        UInt8[] reply = new UInt8[bytes.Length];
        Int32 total = 0;
        while (total < reply.Length)
        {
            Int32 n = await stream.ReadAsync(reply, total, reply.Length - total);
            if (n == 0)
                break;

            total += n;
        }

        client.Close();
        return Encoding.UTF8.GetString(reply, 0, total);
    }

    // Every client connects, then all of them send together: the server serves 200 connections that are open at the same time.
    static Task<Int32> Many(Int32 port, Int32 count)
    {
        TcpClient[] clients = new TcpClient[count];
        for (Int32 k = 0; k < count; k++)
        {
            clients[k] = new TcpClient();
            await clients[k].ConnectAsync("127.0.0.1", port);
        }

        Task<Int32>[] answers = new Task<Int32>[count];
        for (Int32 k = 0; k < count; k++)
            answers[k] = Exchange(clients[k], k + 1);

        Int32 sum = 0;
        for (Int32 k = 0; k < count; k++)
            sum += await answers[k];

        return sum;
    }

    static Task<Int32> Exchange(TcpClient client, Int32 number)
    {
        NetworkStream stream = client.GetStream();
        UInt8[] one = new UInt8[1];
        one[0] = (UInt8)number;

        await stream.WriteAsync(one, 0, 1);

        UInt8[] back = new UInt8[1];
        Int32 n = await stream.ReadAsync(back, 0, 1);
        client.Close();

        return n == 1 ? back[0] : -1;
    }

    [EntryPoint]
    public static void Main()
    {
        IPEndPoint endPoint = new IPEndPoint(IPAddress.Loopback, 80);
        Console.WriteLine("address " + endPoint + " " + IPAddress.Loopback.Equals(IPAddress.Parse("127.0.0.1"))
                          + " " + IPAddress.Loopback.Equals(IPAddress.Any));

        IPAddress? parsed;
        Console.WriteLine("parse " + IPAddress.TryParse("1.2.3", out parsed) + " " + IPAddress.TryParse("1.2.3.300", out parsed)
                          + " " + IPAddress.TryParse("10.0.0.255", out parsed));

        TcpListener listener = new TcpListener(IPAddress.Loopback, 0);
        listener.Start();
        Int32 port = listener.LocalEndpoint.Port;

        // The blocking methods, with a thread for the server.
        Delegate<Void> server = () => {
            TcpClient accepted = listener.AcceptTcpClient();
            NetworkStream s = accepted.GetStream();
            UInt8[] buffer = new UInt8[64];
            Int32 n = s.Read(buffer, 0, buffer.Length);
            s.Write(buffer, 0, n);
            accepted.Close();
        };

        Thread thread = new Thread(server);
        thread.Start();

        TcpClient client = new TcpClient("127.0.0.1", port);
        NetworkStream stream = client.GetStream();
        UInt8[] hello = Encoding.UTF8.GetBytes("hello");
        stream.Write(hello, 0, hello.Length);

        UInt8[] reply = new UInt8[5];
        Int32 got = 0;
        while (got < 5)
            got += stream.Read(reply, got, 5 - got);

        Console.WriteLine("sync echo " + Encoding.UTF8.GetString(reply));
        client.Close();
        thread.Join();
        listener.Stop();

        // The async methods.
        TcpListener async = new TcpListener(IPAddress.Loopback, 0);
        async.Start();
        Int32 asyncPort = async.LocalEndpoint.Port;

        Task listening = Listen(async, 2 + 200);
        Console.WriteLine("async echo " + Ask(asyncPort, "ping 1").Result);
        Console.WriteLine("async echo " + Ask(asyncPort, "ping 2").Result);

        Int32 sum = Many(asyncPort, 200).Result;
        Console.WriteLine("many 200 " + sum);
        listening.Wait();
        async.Stop();

        // A port where no one listens.
        TcpListener unused = new TcpListener(IPAddress.Loopback, 0);
        unused.Start();
        Int32 closedPort = unused.LocalEndpoint.Port;
        unused.Stop();

        try { new TcpClient("127.0.0.1", closedPort); }
        catch (SocketException e) { Console.WriteLine("refused SocketException"); }

        // The other side closes: Read returns 0.
        TcpListener closing = new TcpListener(IPAddress.Loopback, 0);
        closing.Start();

        TcpClient peer = new TcpClient("127.0.0.1", closing.LocalEndpoint.Port);
        TcpClient accepted2 = closing.AcceptTcpClient();
        accepted2.Close();

        UInt8[] none = new UInt8[4];
        Console.WriteLine("closed " + peer.GetStream().Read(none, 0, none.Length));
        peer.Close();
        closing.Stop();

        Console.WriteLine("done");
    }
}
