# System.Net: TCP sockets

TCP over IPv4, in `core_lib/System/Net` (`Net`, `Net.Sockets`); the system calls are in `modules/rtlib/socket.cpp` (WinSock on Windows,
BSD sockets elsewhere).

| | |
|---|---|
| `IPAddress`, `IPEndPoint` | `Parse`, `TryParse`, `Loopback`, `Any`, `ToString` |
| `Dns` | `GetHostAddress(name)`, `GetHostAddressAsync(name)` (an address as text is the address; the first IPv4 address of a name) |
| `Socket` | `Bind`, `Listen`, `Accept`, `Connect`, `Send`, `Receive`, `Shutdown`, `Close`; `NoDelay`, `KeepAlive`, `ReuseAddress`, `LocalEndPoint`, `RemoteEndPoint` |
| `TcpListener` | `Start`, `Stop`, `AcceptSocket`, `AcceptTcpClient`, `LocalEndpoint` (the port that the system gave when 0 was asked) |
| `TcpClient` | `Connect(host, port)`, `GetStream()`, `Close`, `Connected`, `NoDelay`, `Client` |
| `NetworkStream` | a `Stream` on a socket: `Write` sends all the bytes, `Read` returns 0 when the other side closed the connection |
| `SocketException` | `ErrorCode`: -2 refused, -3 reset or closed, -4 other, -5 address in use, -6 timed out |

Each method that waits has an `Async` version (`AcceptAsync`, `AcceptTcpClientAsync`, `ConnectAsync`, `SendAsync`, `ReceiveAsync`,
`NetworkStream.ReadAsync/WriteAsync`; a `CancellationToken` ends the wait with `OperationCanceledException`).

```
TcpListener listener = new TcpListener(IPAddress.Loopback, 0);
listener.Start();
for (;;)
{
    TcpClient client = await listener.AcceptTcpClientAsync();
    Serve(client);                    // a Task method: it goes on while the loop accepts the next one
}
```

## How the waits are made

The sockets do not block: a call that cannot go on returns the code "would block", and the library waits for the socket and calls again.

* The methods without `Async` wait in the thread that called them (`Socket_Poll` for the one socket); the lock of the runtime is
  given back while they wait, so the other threads run, also in the native programs that run threads in parallel.
* The `Async` methods are state machines (`docs/async.md`): when the call would block, they `await` a task of the **reactor**. The
  reactor is one background thread that waits for all the sockets that wait (`poll`, or `WSAPoll` on Windows) and ends the tasks of the
  ones that are ready; a byte that is sent to a socket of the loopback wakes it when a wait is added. So a connection that waits does
  **not** hold a thread: the test `test/unittest/core_lib/System/Net/tcp.cs` has 200 connections open at the same time, and the thread pool has
  64 threads at most.
* `Close` ends the waits for the socket (the callers get `ObjectDisposedException`). A `TcpListener.Stop` does the same to
  `Accept`.

The files use the pool (`docs/io.md`): a file is not a thing that can be waited for in the same way on every system.

## Limits

* IPv4 only (`Dns` returns the first IPv4 address); no UDP, no TLS, no unix sockets, no `SocketOptions` other than the three above.
* `Socket` has no timeouts: use a `CancellationToken` with the `Async` methods.
* The name of a machine is resolved by a call that blocks (in the pool for `GetHostAddressAsync`).
* A stream does not do two operations of the same kind at the same time (two `ReceiveAsync` on one socket).
