# System.IO: files, streams, text

The first I/O of the library: the files and the directories, the streams, the text of a stream, and the console input.
The names are those of .NET, the code is in `core_lib/System/IO`, the system calls are in `modules/rtlib/file.cpp`.

| | |
|---|---|
| `Stream` | the base: `Read`, `Write`, `Seek`, `Flush`, `SetLength`, `Length`, `Position`, `CopyTo`, `ReadByte`, `WriteByte` |
| `FileStream` | the bytes of a file: `FileMode` (CreateNew, Create, Open, OpenOrCreate, Truncate, Append), `FileAccess` |
| `MemoryStream` | the bytes in memory (`ToArray`) |
| `StreamReader` / `StreamWriter` | text (UTF-8 by default, or an `Encoding`): `Read`, `ReadLine`, `ReadToEnd`; `Write`, `WriteLine`, `Flush` |
| `File` | `Exists`, `Delete`, `Move`, `Copy`, `Open*`, `Create`, `ReadAllBytes/Text/Lines`, `WriteAllBytes/Text/Lines`, `AppendAllText` |
| `Directory` | `Exists`, `CreateDirectory` (with the parents), `Delete` (`recursive`), `GetFiles`, `GetDirectories`, `GetCurrentDirectory` |
| `Path` | `Combine`, `GetFileName`, `GetDirectoryName`, `GetExtension`, `GetFileNameWithoutExtension`, `ChangeExtension` |
| `Console.ReadLine()` | the next line of the input, `null` at the end |

The errors are exceptions: `FileNotFoundException`, `DirectoryNotFoundException`, `UnauthorizedAccessException` (a directory used as a
file, no permission), `IOException` (the file exists, the directory is not empty, any other error), `ObjectDisposedException` (a closed
stream). `File.Delete` of a file that does not exist does nothing.

## Asynchronous I/O

Every operation has an `Async` version that returns a `Task`: `Stream.ReadAsync/WriteAsync/FlushAsync/CopyToAsync` (a
`CancellationToken` cancels what has not begun), `StreamReader.ReadLineAsync/ReadToEndAsync`, `StreamWriter.WriteAsync/WriteLineAsync/FlushAsync`,
`File.ReadAllBytesAsync/WriteAllBytesAsync/ReadAllTextAsync/WriteAllTextAsync/AppendAllTextAsync/ReadAllLinesAsync`.

```
String text = await File.ReadAllTextAsync(path);
await source.CopyToAsync(target);
```

They are made by the thread pool: the operation of the stream (the system call) is the work that the pool runs, the method that
awaits it goes on when it ends (`docs/async.md`). So a waiting operation holds a thread of the pool, and the program that makes many at
once is limited by the maximum of the pool. The calls are the same ones that the other back ends use, so the interpreter, the JIT
and the native programs give the same results; the sockets (`docs/net.md`) use a reactor instead, because a connection can wait for a very long time.

A call that waits for the system gives the lock of the runtime back while it waits (`thread_manager_t::blocking`), so the other threads
run (also in the native programs that run the threads in parallel): a thread that reads a file does not stop the others.

A stream does not do two operations at the same time: wait for the one that was started before starting another.

## Limits

* `StreamReader` reads all the bytes that are left when the first read is made, and decodes them at once (the characters made of
  many bytes are never cut); the text is then given by line. A very big file is a big string.
* `StreamWriter` keeps the text until `Flush` or `Close` (`AutoFlush` flushes after each write). Close it (or use `using`): the
  text that is not flushed is lost.
* No sockets, no pipes, no `FileShare`, no file attributes, no `FileInfo` / `DirectoryInfo`, no file watchers.
* The file names are the ones of the system (the paths of Windows have `\`, `Path` accepts both separators).
