/*
 *  System.dll
 *
 *  MemoryStream: a stream of bytes in the memory.
 */

namespace IO;

////////// ////////// ////////// ////////// //////////

public class MemoryStream : Stream
{
    private UInt8[] __bytes;
    private Int32 __length;
    private Int32 __position;
    private Boolean __closed;

    public MemoryStream()
    {
        __bytes = new UInt8[256];
    }

    // A stream that has the bytes (copied), the position is the beginning.
    public MemoryStream(UInt8[] bytes)
    {
        __bytes = new UInt8[bytes.Length > 16 ? bytes.Length : 16];
        Bytes.Copy(bytes, 0, __bytes, 0, bytes.Length);
        __length = bytes.Length;
    }

    public override Boolean CanRead => !__closed;

    public override Boolean CanWrite => !__closed;

    public override Boolean CanSeek => !__closed;

    public override Int64 Length => __length;

    public override Int64 Position
    {
        get { return __position; }
        set
        {
            if (value < 0 || value > 2147483647)
                throw new OutOfRangeException();

            __position = (Int32)value;
        }
    }

    // A copy of the bytes.
    public UInt8[] ToArray()
    {
        UInt8[] result = new UInt8[__length];
        Bytes.Copy(__bytes, 0, result, 0, __length);

        return result;
    }

    public override Int32 Read(UInt8[] buffer, Int32 offset, Int32 count)
    {
        __Check();

        if (offset < 0 || count < 0 || offset > buffer.Length - count)
            throw new OutOfRangeException();

        Int32 n = __length - __position;
        if (n > count)
            n = count;

        if (n <= 0)
            return 0;

        Bytes.Copy(__bytes, __position, buffer, offset, n);
        __position = __position + n;

        return n;
    }

    public override Void Write(UInt8[] buffer, Int32 offset, Int32 count)
    {
        __Check();

        if (offset < 0 || count < 0 || offset > buffer.Length - count)
            throw new OutOfRangeException();

        __Reserve(__position + count);

        // A position after the end: the bytes between are zeros.
        for (Int32 k = __length; k < __position; k++)
            __bytes[k] = 0;

        Bytes.Copy(buffer, offset, __bytes, __position, count);

        __position = __position + count;
        if (__position > __length)
            __length = __position;
    }

    public override Void Flush()
    {
    }

    public override Int64 Seek(Int64 offset, SeekOrigin origin)
    {
        __Check();

        Int64 target = origin == SeekOrigin.Begin ? offset : origin == SeekOrigin.Current ? __position + offset : __length + offset;
        if (target < 0 || target > 2147483647)
            throw new IOException("cannot seek to that position");

        __position = (Int32)target;
        return target;
    }

    public override Void SetLength(Int64 value)
    {
        __Check();

        if (value < 0 || value > 2147483647)
            throw new OutOfRangeException();

        Int32 length = (Int32)value;
        __Reserve(length);

        // The bytes that appear are zeros.
        for (Int32 k = __length; k < length; k++)
            __bytes[k] = 0;

        __length = length;
        if (__position > length)
            __position = length;
    }

    public override Void Close()
    {
        __closed = true;
    }

    private Void __Check()
    {
        if (__closed)
            throw new ObjectDisposedException("the stream is closed");
    }

    private Void __Reserve(Int32 size)
    {
        if (size <= __bytes.Length)
            return;

        Int32 capacity = __bytes.Length * 2;
        if (capacity < size)
            capacity = size;

        UInt8[] bigger = new UInt8[capacity];
        Bytes.Copy(__bytes, 0, bigger, 0, __length);
        __bytes = bigger;
    }
}
