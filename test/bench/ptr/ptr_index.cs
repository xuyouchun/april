import System;
using System;
using System.Runtime;
class Program
{
    [EntryPoint]
    public static void Main()
    {
        Int32 n = 4096;
        Ptr<Int32> a = NativeMemory.Alloc(n * 4).As<Int32>();
        for (Int32 i = 0; i < n; i++)
            a[i] = i;

        Int64 total = 0;
        for (Int32 r = 0; r < 50000; r++)
        {
            Int32 s = 0;
            for (Int32 i = 0; i < n; i++)
                s += a[i];

            total += s;
        }

        Console.WriteLine(total);
        NativeMemory.Free(a);
    }
}
