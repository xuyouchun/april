/*
 *  System.dll
 *
 *  The exceptions of System.IO.
 */

namespace IO;

////////// ////////// ////////// ////////// //////////

// An error of the system when a file, a directory or a stream was used.
public class IOException : SystemException
{
    public IOException() { }

    public IOException(String? message) : base(message) { }
}

public class FileNotFoundException : IOException
{
    public FileNotFoundException() { }

    public FileNotFoundException(String? message) : base(message) { }
}

public class DirectoryNotFoundException : IOException
{
    public DirectoryNotFoundException() { }

    public DirectoryNotFoundException(String? message) : base(message) { }
}

// The file is not allowed to be used that way (a directory, no permission).
public class UnauthorizedAccessException : SystemException
{
    public UnauthorizedAccessException() { }

    public UnauthorizedAccessException(String? message) : base(message) { }
}

public class EndOfStreamException : IOException
{
    public EndOfStreamException() { }

    public EndOfStreamException(String? message) : base(message) { }
}

////////// ////////// ////////// ////////// //////////

// Makes the exception of the code that the functions of the files return.
internal static class IoError
{
    // Throws the exception of a negative code; does nothing for the others.
    internal static Void Check(Int64 code, String path)
    {
        if (code >= 0)
            return;

        // (The code of a handle is a small negative number: the switch is made on 32 bits.)
        switch ((Int32)code)
        {
            case -1:
                throw new FileNotFoundException("could not find the file or the directory '" + path + "'");

            case -2:
                throw new IOException("'" + path + "' exists (or the directory is not empty)");

            case -3:
                throw new UnauthorizedAccessException("access to '" + path + "' is denied");

            case -5:
                throw new DirectoryNotFoundException("could not find the directory of '" + path + "'");

            default:
                throw new IOException("error of the system with '" + path + "'");
        }
    }
}
