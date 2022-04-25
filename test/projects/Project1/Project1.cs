import System;

using System;
using System.Diagnostics;

class Project1
{
    [EntryPoint, Trace]
    // [EntryPoint]
    public static void Main()
    {
        var tuple = new Tuple<int, double>(1, 11.1f);
        var r = tuple.Item2;

        Console.WriteLine(r);
    }
}

class Class1<T>
{
    public Class1(T value)
    {

    }
}

