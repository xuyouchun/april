
import System;

using System;
using System.Diagnostics;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
        new Class1(100).Execute();
    }
};

class Class1
{
    typedef System.Delegate<int, int, int> __Func;

    public Class1(int value)
    {
        __value = value;
    }

    private int __value;

    [Trace]
    public void Execute()
    {
        var func = __Add;
        int r = func(10, 20);

        Console.WriteLine(r);
    }

    private static int __Add(int a, int b)
    {
        return a + b;
    }
};

