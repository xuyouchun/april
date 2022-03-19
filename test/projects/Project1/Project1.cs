import System;

using System;
using System.Diagnostics;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
        BaseClass obj1 = new MyClass1();
        Console.WriteLine(obj1.GetValue());

        BaseClass obj2 = new MyClass2();
        Console.WriteLine(obj2.GetValue());
        Console.WriteLine(obj2.GetValue());
    }
}

interface IMyInterface
{
    int GetValue { get; }
}

class BaseClass
{
    public virtual int GetValue() { return 1; }
}

class MyClass1 : BaseClass
{
    public MyClass1()
    {
        Console.WriteLine("Call MyClass1");
    }

    public override int GetValue() { return 20; }
}

class MyClass2 : BaseClass
{
    public MyClass2()
    {
        Console.WriteLine("Call MyClass2");
    }

    public override int GetValue() { return 3; }
}


