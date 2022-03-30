import System;

using System;
using System.Diagnostics;

class Project1
{
    [EntryPoint, Trace]
    public static void Main()
    {
        IMyInterface obj1 = new MyClass();
        int r = obj1.GetValue1();

        Console.WriteLine(r);
    }
}

interface IMyInterface
{
    int GetValue1();
}

class MyClassBase : IMyInterface
{
    public virtual int GetValue1() { return 1; }
}

class MyClass : MyClassBase
{
    public override int GetValue1() { return 100; }
}


