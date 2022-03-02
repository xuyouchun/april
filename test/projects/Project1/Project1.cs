import System;

using System;
using System.Diagnostics;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
        BaseClass obj = new MyClass();
        int r = obj.GetValue(1);

        Console.WriteLine(r);
    }
}

interface IMyInterface
{
    int GetValue { get; }
}

class BaseClass
{
    public int GetValue(int b) { return 100; }
}

class MyClass : BaseClass
{
    public sealed override int GetValue(int b)
    {
        return 2;
    }
}


