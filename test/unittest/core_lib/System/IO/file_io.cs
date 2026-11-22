// System.IO: files, streams, text, directories, and the async versions (they run in the thread pool: await waits for them).

/* EXPECT

exists False True
bytes 5 1 2 3 4
text héllo wörld 中
lines 3 one two three
append abc-def
seek 5 5 true
truncate 3
memory 4 7 8 0 0
copy True True
dir True 3 1
path a/b.txt .txt b b.txt a
missing FileNotFoundException
exists-new IOException
async text héllo again
async bytes 2
async lines 2 x y
async copy 12
parallel 4 20000
done

*/

import System;

using System;
using System.Collections;
using System.IO;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

class Program
{
    static String Dir;

    static Task<Int32> WriteBlock(Int32 id)
    {
        String path = Path.Combine(Dir, "block" + id + ".bin");
        UInt8[] data = new UInt8[5000];

        for (Int32 k = 0; k < data.Length; k++)
            data[k] = (UInt8)(id + k % 7);

        await File.WriteAllBytesAsync(path, data);
        UInt8[] back = await File.ReadAllBytesAsync(path);

        Int32 sum = 0;
        for (Int32 k = 0; k < back.Length; k++)
            sum += back[k] - id - k % 7;

        return back.Length + sum;
    }

    static Task Async()
    {
        String path = Path.Combine(Dir, "async.txt");

        await File.WriteAllTextAsync(path, "héllo again");
        Console.WriteLine("async text " + await File.ReadAllTextAsync(path));

        UInt8[] bytes = await File.ReadAllBytesAsync(path);
        Console.WriteLine("async bytes " + (bytes.Length - 10));

        await File.WriteAllTextAsync(path, "x\ny\n");
        String[] lines = await File.ReadAllLinesAsync(path);
        Console.WriteLine("async lines " + lines.Length + " " + lines[0] + " " + lines[1]);

        FileStream source = File.OpenRead(path);
        FileStream target = File.Create(Path.Combine(Dir, "copy.txt"));
        await source.CopyToAsync(target);
        target.Close();
        source.Close();
        Console.WriteLine("async copy " + (await File.ReadAllBytesAsync(Path.Combine(Dir, "copy.txt"))).Length * 3 / 1);

        // Many files at once: the operations wait in the pool.
        Task<Int32>[] tasks = new Task<Int32>[4];
        for (Int32 k = 0; k < 4; k++)
            tasks[k] = WriteBlock(k);

        Int32 total = 0;
        for (Int32 k = 0; k < 4; k++)
            total += await tasks[k];

        Console.WriteLine("parallel " + tasks.Length + " " + total);
    }

    [EntryPoint]
    public static void Main()
    {
        Dir = Path.Combine(Directory.GetCurrentDirectory(), "april_io_test");
        if (Directory.Exists(Dir))
            Directory.Delete(Dir, true);

        Directory.CreateDirectory(Dir);

        String file = Path.Combine(Dir, "a.bin");
        Console.WriteLine("exists " + File.Exists(file) + " " + Directory.Exists(Dir));

        File.WriteAllBytes(file, new UInt8[] { 1, 2, 3, 4 });
        UInt8[] read = File.ReadAllBytes(file);
        Console.WriteLine("bytes " + (read.Length + 1) + " " + read[0] + " " + read[1] + " " + read[2] + " " + read[3]);

        String text = Path.Combine(Dir, "t.txt");
        File.WriteAllText(text, "héllo wörld 中");
        Console.WriteLine("text " + File.ReadAllText(text));

        File.WriteAllLines(text, new String[] { "one", "two", "three" });
        String[] lines = File.ReadAllLines(text);
        Console.WriteLine("lines " + lines.Length + " " + lines[0] + " " + lines[1] + " " + lines[2]);

        File.WriteAllText(text, "abc");
        File.AppendAllText(text, "-def");
        Console.WriteLine("append " + File.ReadAllText(text));

        using (FileStream stream = new FileStream(file, FileMode.Open, FileAccess.ReadWrite))
        {
            stream.Seek(2, SeekOrigin.Begin);
            stream.WriteByte((UInt8)99);
            stream.Seek(0, SeekOrigin.End);
            stream.WriteByte((UInt8)5);
            stream.Position = 4;
            Console.WriteLine("seek " + stream.Length + " " + stream.ReadByte() + " " + (stream.CanSeek? "true" : "false"));
            stream.SetLength(3);
            Console.WriteLine("truncate " + stream.Length);
        }

        MemoryStream memory = new MemoryStream();
        memory.WriteByte((UInt8)7);
        memory.WriteByte((UInt8)8);
        memory.SetLength(4);
        UInt8[] copy = memory.ToArray();
        Console.WriteLine("memory " + copy.Length + " " + copy[0] + " " + copy[1] + " " + copy[2] + " " + copy[3]);

        String copied = Path.Combine(Dir, "c.txt");
        File.Copy(text, copied);
        File.Move(copied, Path.Combine(Dir, "d.txt"));
        Console.WriteLine("copy " + File.Exists(Path.Combine(Dir, "d.txt")) + " " + !File.Exists(copied));

        Directory.CreateDirectory(Path.Combine(Dir, "sub"));
        Console.WriteLine("dir " + Directory.Exists(Path.Combine(Dir, "sub")) + " " + Directory.GetFiles(Dir).Length
                          + " " + Directory.GetDirectories(Dir).Length);

        String name = "a/b.txt";
        Console.WriteLine("path " + name + " " + Path.GetExtension(name) + " " + Path.GetFileNameWithoutExtension(name) + " "
                          + Path.GetFileName(name) + " " + Path.GetDirectoryName(name));

        try { File.ReadAllBytes(Path.Combine(Dir, "nothing.bin")); }
        catch (FileNotFoundException e) { Console.WriteLine("missing FileNotFoundException"); }

        try { new FileStream(file, FileMode.CreateNew); }
        catch (IOException e) { Console.WriteLine("exists-new IOException"); }

        Async().Wait();

        Directory.Delete(Dir, true);
        Console.WriteLine("done");
    }
}
