/*
 *  System.dll
 *
 *  FileMode, FileAccess, SeekOrigin.
 */

namespace IO;

////////// ////////// ////////// ////////// //////////

// What happens when a file is opened.
public enum FileMode
{
    CreateNew = 1,          // A new file: IOException if it exists.
    Create = 2,             // A new file, or the one that exists is emptied.
    Open = 3,               // The file that exists: FileNotFoundException if it does not.
    OpenOrCreate = 4,
    Truncate = 5,           // The file that exists, emptied.
    Append = 6,             // The end of the file (it is written only).
}

public enum FileAccess
{
    Read = 1,
    Write = 2,
    ReadWrite = 3,
}

public enum SeekOrigin
{
    Begin = 0,
    Current = 1,
    End = 2,
}
