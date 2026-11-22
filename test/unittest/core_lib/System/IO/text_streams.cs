// StreamReader and StreamWriter: the text of a stream (a mark of UTF-8 at the beginning is not text), the lines end with \n or \r\n,
// the async versions, and the errors of the files.

/* EXPECT

lines 3 first|second|third
rest tail
eof True
read 104 105 -1
bom hi
writer a1b
async lines 2 x|y
async write 12
cancel True
denied UnauthorizedAccessException
no-directory DirectoryNotFoundException
closed ObjectDisposedException

*/

import System;

using System;
using System.IO;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

class Program
{
    static Task Async(String dir)
    {
        String path = Path.Combine(dir, "w.txt");

        StreamWriter writer = new StreamWriter(path);
        await writer.WriteLineAsync("x");
        await writer.WriteAsync("y");
        await writer.FlushAsync();
        writer.Close();

        StreamReader reader = new StreamReader(path);
        String? a = await reader.ReadLineAsync();
        String? b = await reader.ReadLineAsync();
        String? c = await reader.ReadLineAsync();
        reader.Close();
        Console.WriteLine("async lines " + (c == null? 2 : 3) + " " + a + "|" + b);

        FileStream stream = File.Create(Path.Combine(dir, "raw.bin"));
        UInt8[] block = new UInt8[12];
        await stream.WriteAsync(block, 0, block.Length);
        await stream.FlushAsync();
        Console.WriteLine("async write " + stream.Length);
        stream.Close();

        CancellationTokenSource source = new CancellationTokenSource();
        source.Cancel();

        FileStream again = File.OpenRead(Path.Combine(dir, "raw.bin"));
        try { await again.ReadAsync(block, 0, 4, source.Token); }
        catch (OperationCanceledException e) { Console.WriteLine("cancel " + true); }
        again.Close();
    }

    [EntryPoint]
    public static void Main()
    {
        String dir = Path.Combine(Directory.GetCurrentDirectory(), "april_io_test2");
        if (Directory.Exists(dir))
            Directory.Delete(dir, true);

        Directory.CreateDirectory(dir);

        MemoryStream memory = new MemoryStream(Encoding.UTF8.GetBytes("first\r\nsecond\nthird\ntail"));
        StreamReader reader = new StreamReader(memory);

        String l1 = reader.ReadLine();
        String l2 = reader.ReadLine();
        String l3 = reader.ReadLine();
        Console.WriteLine("lines 3 " + l1 + "|" + l2 + "|" + l3);
        Console.WriteLine("rest " + reader.ReadToEnd());
        Console.WriteLine("eof " + reader.EndOfStream);

        StreamReader chars = new StreamReader(new MemoryStream(Encoding.UTF8.GetBytes("hi")));
        Int32 c1 = chars.Read();
        Int32 c2 = chars.Read();
        Console.WriteLine("read " + c1 + " " + c2 + " " + chars.Read());

        UInt8[] marked = new UInt8[] { 239, 187, 191, 104, 105 };
        Console.WriteLine("bom " + new StreamReader(new MemoryStream(marked)).ReadToEnd());

        MemoryStream sink = new MemoryStream();
        StreamWriter writer = new StreamWriter(sink);
        writer.Write("a");
        writer.Write(1);
        writer.WriteLine();
        writer.Write("b");
        writer.Flush();
        String written = Encoding.UTF8.GetString(sink.ToArray());
        Console.WriteLine("writer " + written.Replace("\n", " ").Trim().Replace(" ", ""));

        Async(dir).Wait();

        try { new FileStream(dir, FileMode.Open, FileAccess.Read); }
        catch (UnauthorizedAccessException e) { Console.WriteLine("denied UnauthorizedAccessException"); }

        try { File.WriteAllText(Path.Combine(Path.Combine(dir, "missing"), "f.txt"), "x"); }
        catch (DirectoryNotFoundException e) { Console.WriteLine("no-directory DirectoryNotFoundException"); }

        FileStream closed = File.Create(Path.Combine(dir, "closed.bin"));
        closed.Close();
        try { closed.ReadByte(); }
        catch (ObjectDisposedException e) { Console.WriteLine("closed ObjectDisposedException"); }

        Directory.Delete(dir, true);
    }
}
