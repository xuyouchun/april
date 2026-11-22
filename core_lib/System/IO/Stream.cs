/*
 *  System.dll
 *
 *  Stream: a sequence of bytes that is read or written (FileStream, MemoryStream).
 */

using Threading;
using Threading.Tasks;

namespace IO;

////////// ////////// ////////// ////////// //////////

// Copies bytes between arrays (the ranges are checked by the caller).
internal static class Bytes
{
    internal static Void Copy(UInt8[] source, Int32 sourceIndex, UInt8[] destination, Int32 destinationIndex, Int32 count)
    {
        if (count > 0)
            Internal.Memory_Copy(Ptr.AddressOf(destination, destinationIndex), Ptr.AddressOf(source, sourceIndex), count);
    }
}

////////// ////////// ////////// ////////// //////////

// The base of the streams. The *Async methods do the same as the others in the thread pool (a stream does not do two operations
// at the same time: wait for the one that was started before starting another).
public abstract class Stream : IDisposable
{
    public abstract Boolean CanRead { get; }

    public abstract Boolean CanWrite { get; }

    public abstract Boolean CanSeek { get; }

    public abstract Int64 Length { get; }

    public abstract Int64 Position { get; set; }

    // Reads up to count bytes to the buffer, returns how many (0 at the end of the stream).
    public abstract Int32 Read(UInt8[] buffer, Int32 offset, Int32 count);

    public abstract Void Write(UInt8[] buffer, Int32 offset, Int32 count);

    public abstract Void Flush();

    public abstract Int64 Seek(Int64 offset, SeekOrigin origin);

    public abstract Void SetLength(Int64 value);

    // Gives back the file or the memory that the stream holds.
    public virtual Void Close()
    {
    }

    public Void Dispose()
    {
        Close();
    }

    // The next byte, -1 at the end of the stream.
    public Int32 ReadByte()
    {
        UInt8[] one = new UInt8[1];
        return Read(one, 0, 1) == 0 ? -1 : one[0];
    }

    public Void WriteByte(UInt8 value)
    {
        UInt8[] one = new UInt8[1];
        one[0] = value;
        Write(one, 0, 1);
    }

    // Copies the bytes from here to the end to another stream.
    public Void CopyTo(Stream destination)
    {
        UInt8[] buffer = new UInt8[81920];

        for (;;)
        {
            Int32 n = Read(buffer, 0, buffer.Length);
            if (n == 0)
                return;

            destination.Write(buffer, 0, n);
        }
    }

    public Task<Int32> ReadAsync(UInt8[] buffer, Int32 offset, Int32 count)
    {
        return ReadAsync(buffer, offset, count, CancellationToken.None);
    }

    // The token cancels the operation if it has not begun.
    public virtual Task<Int32> ReadAsync(UInt8[] buffer, Int32 offset, Int32 count, CancellationToken cancellationToken)
    {
        StreamWork work = new StreamWork(this, 0, buffer, offset, count);
        Delegate<Int32> run = work.Run;

        return Task.Run(run, cancellationToken);
    }

    public Task WriteAsync(UInt8[] buffer, Int32 offset, Int32 count)
    {
        return WriteAsync(buffer, offset, count, CancellationToken.None);
    }

    public virtual Task WriteAsync(UInt8[] buffer, Int32 offset, Int32 count, CancellationToken cancellationToken)
    {
        StreamWork work = new StreamWork(this, 1, buffer, offset, count);
        Delegate<Int32> run = work.Run;

        return Task.Run(run, cancellationToken);
    }

    public Task FlushAsync()
    {
        StreamWork work = new StreamWork(this, 2, new UInt8[0], 0, 0);
        Delegate<Int32> run = work.Run;

        return Task.Run(run, CancellationToken.None);
    }

    public Task CopyToAsync(Stream destination)
    {
        UInt8[] buffer = new UInt8[81920];

        for (;;)
        {
            Int32 n = await ReadAsync(buffer, 0, buffer.Length);
            if (n == 0)
                return;

            await destination.WriteAsync(buffer, 0, n);
        }
    }
}

////////// ////////// ////////// ////////// //////////

// The operation of a stream that the pool does.
internal class StreamWork
{
    private Stream __stream;
    private Int32 __kind;
    private UInt8[] __buffer;
    private Int32 __offset;
    private Int32 __count;

    internal StreamWork(Stream stream, Int32 kind, UInt8[] buffer, Int32 offset, Int32 count)
    {
        __stream = stream;
        __kind = kind;
        __buffer = buffer;
        __offset = offset;
        __count = count;
    }

    internal Int32 Run()
    {
        switch (__kind)
        {
            case 0:
                return __stream.Read(__buffer, __offset, __count);

            case 1:
                __stream.Write(__buffer, __offset, __count);
                return 0;

            default:
                __stream.Flush();
                return 0;
        }
    }
}
