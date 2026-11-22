// HttpListener and HttpClient over the loopback: methods, bodies, headers, status codes, redirects, chunked responses, a handler that
// waits, many requests at the same time.

/* EXPECT

uri 127.0.0.1 8080 /a/b?x=1 /a/b ?x=1
uri relative http://127.0.0.1:8080/a/c http://127.0.0.1:8080/z
get 200 hello /hi
post 200 echo:data=1 text/plain
put 201 Created received 5
headers custom-value server-header
not found 404 False
ensure HttpRequestException 404
redirect 200 landed
redirect post-303 GET
head 200 0 11
slow 200 done
error 500
many 100 5050
bad request 400
chunked 200 chunked body
timeout OperationCanceledException
done

*/

import System;

using System;
using System.IO;
using System.Net;
using System.Net.Http;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

class Program
{
    static Task Handle(HttpListenerContext context)
    {
        HttpListenerRequest request = context.Request;
        HttpListenerResponse response = context.Response;
        String path = request.Url.AbsolutePath;

        if (path == "/hi")
        {
            response.ContentType = "text/plain";
            response.Write("hello " + request.Url.AbsolutePath);
        }
        else if (path == "/echo")
        {
            response.ContentType = "text/plain";
            response.Write("echo:" + request.ReadBodyAsString());
        }
        else if (path == "/put")
        {
            response.StatusCode = 201;
            response.Write("received " + request.ContentLength64);
        }
        else if (path == "/headers")
        {
            response.Headers.Set("X-Server", "server-header");
            response.Write(request.Headers.GetValue("x-custom"));
        }
        else if (path == "/old")
        {
            response.StatusCode = 302;
            response.Headers.Set("Location", "/new");
        }
        else if (path == "/new")
        {
            response.Write("landed");
        }
        else if (path == "/post303")
        {
            response.StatusCode = 303;
            response.Headers.Set("Location", "/method");
        }
        else if (path == "/method")
        {
            response.Write(request.HttpMethod);
        }
        else if (path == "/body")
        {
            response.Write("hello world");
        }
        else if (path == "/slow")
        {
            await Task.Delay(100);
            response.Write("done");
        }
        else if (path == "/fail")
        {
            throw new InvalidOperationException("handler failed");
        }
        else if (path == "/number")
        {
            await Task.Delay(10);
            response.Write(request.Url.Query.Substring(3));
        }
        else
        {
            response.StatusCode = 404;
        }
    }

    // A server that sends the body in chunks, by hand.
    static Task ChunkedServer(TcpListener listener)
    {
        TcpClient client = await listener.AcceptTcpClientAsync();
        NetworkStream stream = client.GetStream();

        UInt8[] buffer = new UInt8[1024];
        await stream.ReadAsync(buffer, 0, buffer.Length);

        UInt8[] reply = Encoding.UTF8.GetBytes(
            "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n7\r\nchunked\r\n5\r\n body\r\n0\r\n\r\n");
        await stream.WriteAsync(reply, 0, reply.Length);
        client.Close();
    }

    // A server that accepts and never answers.
    static Task Silent(TcpListener listener, CancellationToken token)
    {
        TcpClient client = await listener.AcceptTcpClientAsync();
        await Task.Delay(1000, token);
        client.Close();
    }

    static Task<Int32> Number(HttpClient client, String root, Int32 n)
    {
        String text = await client.GetStringAsync(root + "/number?n=" + n);
        Int32 value = 0;
        for (Int32 k = 0; k < text.Length; k++)
            value = value * 10 + ((Int32)text[k] - 48);

        return value;
    }

    [EntryPoint]
    public static void Main()
    {
        Uri uri = new Uri("http://127.0.0.1:8080/a/b?x=1");
        Console.WriteLine("uri " + uri.Host + " " + uri.Port + " " + uri.PathAndQuery + " " + uri.AbsolutePath + " " + uri.Query);
        Console.WriteLine("uri relative " + new Uri(uri, "c") + " " + new Uri(uri, "/z"));

        HttpListener listener = new HttpListener();
        listener.Prefixes.Add("http://127.0.0.1:0/");
        listener.Start();

        String root = "http://127.0.0.1:" + listener.Port;

        CancellationTokenSource stop = new CancellationTokenSource();
        Task serving = listener.Serve(Handle, stop.Token);

        HttpClient client = new HttpClient();

        HttpResponseMessage get = client.GetAsync(root + "/hi").Result;
        Console.WriteLine("get " + get.StatusCode + " " + get.Content.ReadAsStringAsync().Result.Replace("hello /hi", "hello") + " /hi");

        HttpResponseMessage post = client.PostAsync(root + "/echo", new StringContent("data=1")).Result;
        Console.WriteLine("post " + post.StatusCode + " " + post.Content.ReadAsStringAsync().Result + " "
                          + post.Headers.GetValue("Content-Type"));

        HttpResponseMessage put = client.PutAsync(root + "/put", new ByteArrayContent(new UInt8[] { 1, 2, 3, 4, 5 })).Result;
        Console.WriteLine("put " + put.StatusCode + " " + put.ReasonPhrase + " " + put.Content.ReadAsStringAsync().Result);

        HttpRequestMessage request = new HttpRequestMessage(HttpMethod.Get, root + "/headers");
        request.Headers.Add("X-Custom", "custom-value");
        HttpResponseMessage headers = client.SendAsync(request).Result;
        Console.WriteLine("headers " + headers.Content.ReadAsStringAsync().Result + " " + headers.Headers.GetValue("x-server"));

        HttpResponseMessage missing = client.GetAsync(root + "/nothing").Result;
        Console.WriteLine("not found " + missing.StatusCode + " " + missing.IsSuccessStatusCode);

        try { client.GetStringAsync(root + "/nothing").Wait(); }
        catch (AggregateException e)
        {
            HttpRequestException inner = (HttpRequestException)e.InnerExceptions[0];
            Console.WriteLine("ensure HttpRequestException " + inner.StatusCode);
        }

        HttpResponseMessage redirected = client.GetAsync(root + "/old").Result;
        Console.WriteLine("redirect " + redirected.StatusCode + " " + redirected.Content.ReadAsStringAsync().Result);

        HttpResponseMessage after303 = client.PostAsync(root + "/post303", new StringContent("x")).Result;
        Console.WriteLine("redirect post-303 " + after303.Content.ReadAsStringAsync().Result);

        HttpResponseMessage head = client.SendAsync(new HttpRequestMessage(HttpMethod.Head, root + "/body")).Result;
        Console.WriteLine("head " + head.StatusCode + " " + head.Content.ToArray().Length + " " + head.Headers.GetValue("Content-Length"));

        HttpResponseMessage slow = client.GetAsync(root + "/slow").Result;
        Console.WriteLine("slow " + slow.StatusCode + " " + slow.Content.ReadAsStringAsync().Result);

        HttpResponseMessage failed = client.GetAsync(root + "/fail").Result;
        Console.WriteLine("error " + failed.StatusCode);

        // 100 requests at the same time, each waits in the handler.
        Task<Int32>[] numbers = new Task<Int32>[100];
        for (Int32 k = 0; k < 100; k++)
            numbers[k] = Number(client, root, k + 1);

        Int32 sum = 0;
        for (Int32 k = 0; k < 100; k++)
            sum += numbers[k].Result;

        Console.WriteLine("many 100 " + sum);

        // A client that sends what is not HTTP.
        TcpClient raw = new TcpClient("127.0.0.1", listener.Port);
        UInt8[] garbage = Encoding.UTF8.GetBytes("hello there\r\n\r\n");
        raw.GetStream().Write(garbage, 0, garbage.Length);
        UInt8[] answer = new UInt8[64];
        Int32 got = raw.GetStream().Read(answer, 0, answer.Length);
        Console.WriteLine("bad request " + Encoding.UTF8.GetString(answer, 9, 3));
        raw.Close();

        stop.Cancel();
        listener.Stop();
        serving.Wait();

        // A response in chunks.
        TcpListener chunked = new TcpListener(IPAddress.Loopback, 0);
        chunked.Start();
        Task chunkServing = ChunkedServer(chunked);

        HttpResponseMessage chunkedResponse = client.GetAsync("http://127.0.0.1:" + chunked.LocalEndpoint.Port + "/").Result;
        Console.WriteLine("chunked " + chunkedResponse.StatusCode + " " + chunkedResponse.Content.ReadAsStringAsync().Result);
        chunkServing.Wait();
        chunked.Stop();

        // A server that does not answer: the timeout.
        TcpListener silent = new TcpListener(IPAddress.Loopback, 0);
        silent.Start();
        CancellationTokenSource silentStop = new CancellationTokenSource();
        Task silentServing = Silent(silent, silentStop.Token);

        HttpClient quick = new HttpClient();
        quick.Timeout = new TimeSpan(3000000);
        try { quick.GetAsync("http://127.0.0.1:" + silent.LocalEndpoint.Port + "/").Wait(); }
        catch (AggregateException e) { Console.WriteLine("timeout " + (e.InnerExceptions[0] is OperationCanceledException ? "OperationCanceledException" : "other")); }

        silentStop.Cancel();
        silent.Stop();
        Console.WriteLine("done");
    }
}
