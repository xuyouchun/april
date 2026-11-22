# System.Net.Http: HttpClient and HttpListener

HTTP/1.1 over the sockets of `docs/net.md`, in `core_lib/System/Net/Http` (and `Net/Uri.cs`). **http only: there is no TLS**
(an `https://` address is a `NotSupportedException`). The interpreter, the JIT and the native programs give the same results
(`test/unittest/core_lib/System/Net/http.cs`).

## Client

```
HttpClient client = new HttpClient();
String text = await client.GetStringAsync("http://127.0.0.1:8080/hi");

HttpResponseMessage response = await client.PostAsync(url, new StringContent("{...}", "application/json"));
response.EnsureSuccessStatusCode();
UInt8[] bytes = await response.Content.ReadAsByteArrayAsync();
```

| | |
|---|---|
| `HttpClient` | `GetAsync`, `PostAsync`, `PutAsync`, `DeleteAsync`, `SendAsync(request[, token])`, `GetStringAsync`, `GetByteArrayAsync`; `BaseAddress`, `Timeout` (100 seconds), `DefaultRequestHeaders`, `MaxAutomaticRedirections` (10), `MaxResponseContentBufferSize` (16 MB) |
| `HttpRequestMessage` | `Method`, `RequestUri`, `Headers`, `Content` |
| `HttpResponseMessage` | `StatusCode` (a number), `ReasonPhrase`, `Headers`, `Content`, `IsSuccessStatusCode`, `EnsureSuccessStatusCode()` |
| `HttpContent` | `ByteArrayContent`, `StringContent(text[, mediaType])` (UTF-8), `ReadAsStringAsync`, `ReadAsByteArrayAsync`, `ReadAsStream`, `Headers` |
| `HttpMethod`, `HttpStatusCode`, `HttpHeaders`, `Uri` | the names are those of .NET; the names of the headers are compared without the case |

* A request opens a connection and closes it after the response (`Connection: close`: no keep-alive, no pool of connections). The
  body of the response is read all: `Content-Length`, `Transfer-Encoding: chunked`, or until the server closes the connection.
* 301, 302, 303 go on with a GET without a body, 307 and 308 repeat the request; the address of `Location` can be relative.
* The time of `Timeout` (and the token of `SendAsync`) cancels the request, also while it connects or waits for the server:
  `OperationCanceledException`. A response that is not a success is not an exception (`EnsureSuccessStatusCode`,
  `GetStringAsync` throw `HttpRequestException` with the `StatusCode`).
* The waits are the ones of the sockets: they do not hold a thread of the pool.

## Server

```
HttpListener listener = new HttpListener();
listener.Prefixes.Add("http://127.0.0.1:8080/");     // port 0: the system gives one (listener.Port)
listener.Start();

await listener.Serve(Handle, token);                  // many requests at the same time

static Task Handle(HttpListenerContext context)
{
    HttpListenerResponse response = context.Response;
    response.ContentType = "text/plain";
    response.Write("hello " + context.Request.Url.AbsolutePath);
}
```

`GetContext()` / `GetContextAsync([token])` give one request (a context that has `Request` and `Response`) when you want the loop.
`Request`: `HttpMethod`, `Url`, `RawUrl`, `Headers`, `ReadBody()`, `ReadBodyAsString()`, `InputStream`, `RemoteEndPoint`.
`Response`: `StatusCode`, `StatusDescription`, `ContentType`, `Headers`, `OutputStream`, `Write(text | bytes)`, `Close()`, `CloseAsync()`.

* The body of the response is kept until `Close` (then `Content-Length` is known). `Serve` closes the response when the handler ends,
  and answers 500 when the handler throws; with `GetContext` you call `Close` yourself.
* A connection that sends what is not an HTTP request gets 400 and is closed (the next one is waited for).
* Each connection has one request: the response has `Connection: close`.

## Limits

* No TLS, no keep-alive, no HTTP/2, no compression (`Content-Encoding`), no cookies, no proxy, no `multipart`, no authentication.
* The bodies are not streamed: a request or a response is read all (16 MB at most), the head is 64 KB at most.
* A response to `HEAD` has the length of the body and not the body; `Expect: 100-continue` is not understood.
* The prefix is used for the address and the port only; the program tells the paths by `Request.Url`.
* No encoding of the characters of a query (`%xx`): the text of the address is used as it is.
