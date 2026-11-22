/*
 *  System.dll
 *
 *  Path, File, Directory.
 */

using Collections;
using Text;
using Threading.Tasks;

namespace IO;

////////// ////////// ////////// ////////// //////////

// The names of the files. Both '/' and '\' separate the names.
public static class Path
{
    private static Char __separator;

    // The separator of the system.
    public static Char DirectorySeparatorChar
    {
        get
        {
            if (__separator == (Char)0)
                __separator = Internal.Directory_Current().IndexOf('\\') >= 0 ? '\\' : '/';

            return __separator;
        }
    }

    private static Boolean __IsSeparator(Char c) => c == '/' || c == '\\';

    // Joins two parts: a second part that is a root (begins with a separator) is the result.
    public static String Combine(String a, String b)
    {
        if (a == null || b == null)
            throw new ArgumentException("a part of the path is null");

        if (a.Length == 0)
            return b;

        if (b.Length == 0)
            return a;

        if (__IsSeparator(b[0]))
            return b;

        if (__IsSeparator(a[a.Length - 1]))
            return a + b;

        return a + DirectorySeparatorChar + b;
    }

    public static String Combine(String a, String b, String c) => Combine(Combine(a, b), c);

    // The index of the last separator, -1 if there is none.
    private static Int32 __LastSeparator(String path)
    {
        for (Int32 k = path.Length - 1; k >= 0; k--)
        {
            if (__IsSeparator(path[k]))
                return k;
        }

        return -1;
    }

    // The name after the last separator.
    public static String GetFileName(String path)
    {
        Int32 k = __LastSeparator(path);
        return path.Substring(k + 1, path.Length - k - 1);
    }

    // What is before the last separator (null for a root or an empty path, "" for a name alone).
    public static String? GetDirectoryName(String path)
    {
        if (path.Length == 0)
            return null;

        Int32 k = __LastSeparator(path);
        if (k < 0)
            return "";

        return k == 0 ? null : path.Substring(0, k);
    }

    // The extension with its dot, "" if there is none.
    public static String GetExtension(String path)
    {
        String name = GetFileName(path);
        Int32 dot = name.LastIndexOf('.');

        return dot < 0 ? "" : name.Substring(dot, name.Length - dot);
    }

    public static String GetFileNameWithoutExtension(String path)
    {
        String name = GetFileName(path);
        Int32 dot = name.LastIndexOf('.');

        return dot < 0 ? name : name.Substring(0, dot);
    }

    public static String ChangeExtension(String path, String? extension)
    {
        String name = GetFileName(path);
        Int32 dot = name.LastIndexOf('.');
        String head = dot < 0 ? path : path.Substring(0, path.Length - (name.Length - dot));

        if (extension == null || extension.Length == 0)
            return head;

        return extension[0] == '.' ? head + extension : head + "." + extension;
    }
}

////////// ////////// ////////// ////////// //////////

// The files: the methods that have Async in the name do the same in the thread pool, and wait for it with await (the file is read
// and written by blocks).
public static class File
{
    public static Boolean Exists(String path) => Internal.File_Exists(path);

    public static Void Delete(String path)
    {
        IoError.Check(Internal.File_Delete(path), path);
    }

    public static Void Move(String source, String destination)
    {
        Move(source, destination, false);
    }

    public static Void Move(String source, String destination, Boolean overwrite)
    {
        IoError.Check(Internal.File_Move(source, destination, overwrite), source);
    }

    public static Void Copy(String source, String destination)
    {
        Copy(source, destination, false);
    }

    public static Void Copy(String source, String destination, Boolean overwrite)
    {
        IoError.Check(Internal.File_Copy(source, destination, overwrite), source);
    }

    public static FileStream Open(String path, FileMode mode) => new FileStream(path, mode);

    public static FileStream Open(String path, FileMode mode, FileAccess access) => new FileStream(path, mode, access);

    public static FileStream OpenRead(String path) => new FileStream(path, FileMode.Open, FileAccess.Read);

    public static FileStream OpenWrite(String path) => new FileStream(path, FileMode.OpenOrCreate, FileAccess.Write);

    public static FileStream Create(String path) => new FileStream(path, FileMode.Create, FileAccess.ReadWrite);

    public static UInt8[] ReadAllBytes(String path)
    {
        using (FileStream stream = OpenRead(path))
        {
            Int32 length = (Int32)stream.Length;
            UInt8[] data = new UInt8[length];
            Int32 total = 0;

            while (total < length)
            {
                Int32 n = stream.Read(data, total, length - total);
                if (n == 0)
                    break;

                total = total + n;
            }

            return data;
        }
    }

    public static Void WriteAllBytes(String path, UInt8[] bytes)
    {
        using (FileStream stream = new FileStream(path, FileMode.Create, FileAccess.Write))
            stream.Write(bytes, 0, bytes.Length);
    }

    public static String ReadAllText(String path) => ReadAllText(path, Encoding.UTF8);

    public static String ReadAllText(String path, Encoding encoding)
    {
        using (StreamReader reader = new StreamReader(OpenRead(path), encoding))
            return reader.ReadToEnd();
    }

    public static Void WriteAllText(String path, String text) => WriteAllText(path, text, Encoding.UTF8);

    public static Void WriteAllText(String path, String text, Encoding encoding)
    {
        WriteAllBytes(path, encoding.GetBytes(text));
    }

    public static Void AppendAllText(String path, String text)
    {
        UInt8[] bytes = Encoding.UTF8.GetBytes(text);

        using (FileStream stream = new FileStream(path, FileMode.Append, FileAccess.Write))
            stream.Write(bytes, 0, bytes.Length);
    }

    public static String[] ReadAllLines(String path)
    {
        List<String> lines = new List<String>();

        using (StreamReader reader = new StreamReader(OpenRead(path)))
        {
            for (String? line = reader.ReadLine(); line != null; line = reader.ReadLine())
                lines.Add(line);
        }

        return lines.ToArray();
    }

    public static Void WriteAllLines(String path, String[] lines)
    {
        StringBuilder text = new StringBuilder();
        for (Int32 k = 0; k < lines.Length; k++)
        {
            text.Append(lines[k]);
            text.Append("\n");
        }

        WriteAllText(path, text.ToString());
    }

    public static Task<UInt8[]> ReadAllBytesAsync(String path)
    {
        FileStream stream = OpenRead(path);

        try
        {
            Int32 length = (Int32)stream.Length;
            UInt8[] data = new UInt8[length];
            Int32 total = 0;

            while (total < length)
            {
                Int32 n = await stream.ReadAsync(data, total, length - total);
                if (n == 0)
                    break;

                total = total + n;
            }

            return data;
        }
        finally
        {
            stream.Close();
        }
    }

    public static Task WriteAllBytesAsync(String path, UInt8[] bytes)
    {
        FileStream stream = new FileStream(path, FileMode.Create, FileAccess.Write);

        try
        {
            await stream.WriteAsync(bytes, 0, bytes.Length);
        }
        finally
        {
            stream.Close();
        }
    }

    public static Task<String> ReadAllTextAsync(String path)
    {
        StreamReader reader = new StreamReader(OpenRead(path), Encoding.UTF8);

        try
        {
            return await reader.ReadToEndAsync();
        }
        finally
        {
            reader.Close();
        }
    }

    public static Task WriteAllTextAsync(String path, String text)
    {
        await WriteAllBytesAsync(path, Encoding.UTF8.GetBytes(text));
    }

    public static Task AppendAllTextAsync(String path, String text)
    {
        UInt8[] bytes = Encoding.UTF8.GetBytes(text);
        FileStream stream = new FileStream(path, FileMode.Append, FileAccess.Write);

        try
        {
            await stream.WriteAsync(bytes, 0, bytes.Length);
        }
        finally
        {
            stream.Close();
        }
    }

    public static Task<String[]> ReadAllLinesAsync(String path)
    {
        List<String> lines = new List<String>();
        StreamReader reader = new StreamReader(OpenRead(path), Encoding.UTF8);

        try
        {
            for (String? line = await reader.ReadLineAsync(); line != null; line = await reader.ReadLineAsync())
                lines.Add(line);
        }
        finally
        {
            reader.Close();
        }

        return lines.ToArray();
    }
}

////////// ////////// ////////// ////////// //////////

public static class Directory
{
    public static Boolean Exists(String path) => Internal.Directory_Exists(path);

    // Makes the directory and the ones before it that do not exist.
    public static Void CreateDirectory(String path)
    {
        IoError.Check(Internal.Directory_Create(path), path);
    }

    // The directory must be empty.
    public static Void Delete(String path) => Delete(path, false);

    public static Void Delete(String path, Boolean recursive)
    {
        IoError.Check(Internal.Directory_Delete(path, recursive), path);
    }

    public static String GetCurrentDirectory() => Internal.Directory_Current();

    // The paths of the files of the directory (not the ones of the directories under it).
    public static String[] GetFiles(String path) => __List(path, 'f');

    public static String[] GetDirectories(String path) => __List(path, 'd');

    private static String[] __List(String path, Char kind)
    {
        String? text = Internal.Directory_List(path);
        if (text == null)
            throw new DirectoryNotFoundException("could not find the directory '" + path + "'");

        List<String> names = new List<String>();
        String[] lines = text.Split('\n');

        for (Int32 k = 0; k < lines.Length; k++)
        {
            String line = lines[k];
            if (line.Length > 2 && line[0] == kind)
                names.Add(Path.Combine(path, line.Substring(2, line.Length - 2)));
        }

        return names.ToArray();
    }
}
