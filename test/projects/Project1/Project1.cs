import System;

using System;
using System.Diagnostics;

class Project1
{
    [EntryPoint, Trace]
    public static void Main()
    {
        int a = 1, b = 2;
        int r = a + b;

        Console.WriteLine(r * 100 + 3);
    }
}

