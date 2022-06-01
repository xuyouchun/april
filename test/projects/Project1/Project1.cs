import System;

using System;
using System.Diagnostics;

class Project1
{
    [EntryPoint, Trace]
    public static void Main()
    {
        var func1 = __MyFunc1;

        int r = func1(100, 200);
        Console.WriteLine(r);
    }

    private static int __MyFunc1(int a, int b)
    {
        return a + b;
    }
}


