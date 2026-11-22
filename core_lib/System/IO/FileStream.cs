/*
 *  System.dll
 *
 *  FileStream: the bytes of a file.
 */

namespace IO;

////////// ////////// ////////// ////////// //////////

public class FileStream : Stream
{
    private Int64 __handle;
    private String __path;
    private FileAccess __access;
    private Boolean __closed;

    public FileStream(String path, FileMode mode)
        : this(path, mode, mode == FileMode.Append ? FileAccess.Write : FileAccess.ReadWrite)
    {
    }

    public FileStream(String path, FileMode mode, FileAccess access)
    {
        if (path == null)
            throw new ArgumentException("the path is null");

        if (mode == FileMode.Append && access != FileAccess.Write)
            throw new ArgumentException("Append needs the access Write");

        if (access == FileAccess.Read && (mode == FileMode.Create || mode == FileMode.CreateNew || mode == FileMode.Truncate
                                            || mode == FileMode.Append))
        {
            throw new ArgumentException("the mode needs to write the file");
        }

        Int64 handle = Internal.File_Open(path, (Int32)mode, (Int32)access);
        IoError.Check(handle, path);

        __handle = handle;
        __path = path;
        __access = access;
    }

    public String Name => __path;

    public override Boolean CanRead => !__closed && __access != FileAccess.Write;

    public override Boolean CanWrite => !__closed && __access != FileAccess.Read;

    public override Boolean CanSeek => !__closed;

    public override Int64 Length
    {
        get
        {
            __Check();

            Int64 length = Internal.File_Length(__handle);
            if (length < 0)
                throw new IOException("cannot get the length of '" + __path + "'");

            return length;
        }
    }

    public override Int64 Position
    {
        get { return Seek(0, SeekOrigin.Current); }
        set { Seek(value, SeekOrigin.Begin); }
    }

    public override Int32 Read(UInt8[] buffer, Int32 offset, Int32 count)
    {
        __Check();
        __CheckRange(buffer, offset, count);

        if (__access == FileAccess.Write)
            throw new NotSupportedException("the stream cannot be read");

        if (count == 0)
            return 0;

        Int32 n = Internal.File_Read(__handle, Ptr.AddressOf(buffer, offset), count);
        IoError.Check(n, __path);

        return n;
    }

    public override Void Write(UInt8[] buffer, Int32 offset, Int32 count)
    {
        __Check();
        __CheckRange(buffer, offset, count);

        if (__access == FileAccess.Read)
            throw new NotSupportedException("the stream cannot be written");

        if (count == 0)
            return;

        Int32 n = Internal.File_Write(__handle, Ptr.AddressOf(buffer, offset), count);
        IoError.Check(n, __path);
    }

    public override Void Flush()
    {
        __Check();
        IoError.Check(Internal.File_Flush(__handle), __path);
    }

    public override Int64 Seek(Int64 offset, SeekOrigin origin)
    {
        __Check();

        Int64 position = Internal.File_Seek(__handle, offset, (Int32)origin);
        if (position < 0)
            throw new IOException("cannot seek to that position of '" + __path + "'");

        return position;
    }

    public override Void SetLength(Int64 value)
    {
        __Check();

        if (value < 0)
            throw new ArgumentException("the length is negative");

        IoError.Check(Internal.File_SetLength(__handle, value), __path);
    }

    public override Void Close()
    {
        if (__closed)
            return;

        __closed = true;
        Internal.File_Close(__handle);
    }

    private Void __Check()
    {
        if (__closed)
            throw new ObjectDisposedException("the file is closed");
    }

    private Void __CheckRange(UInt8[] buffer, Int32 offset, Int32 count)
    {
        if (buffer == null)
            throw new ArgumentException("the buffer is null");

        if (offset < 0 || count < 0 || offset > buffer.Length - count)
            throw new OutOfRangeException();
    }
}
