import System;

using System;
using System.Diagnostics;

class Project1
{
    [EntryPoint, Trace]
    public static void Main()
    {
        object obj = new object(); // new MyClass(20);

        var r = ( (obj as Base) ?? new MyClass(100)).GetValue();
        Console.WriteLine(r);
    }
}

interface IInterface
{
    int GetValue();

    int GetValue<T>();

    int GetValue<T, K>();
}

interface IInterface2 : IInterface
{

}

class Base : IInterface
{
    public virtual int GetValue()
    {
        return 10;
    }

    public virtual int GetValue<T>()
    {
        return 100;
    }

    public virtual int GetValue<T, K>()
    {
        return 1000;
    }
}

class MyClass : Base
{
    public MyClass(int value)
    {
        this.__value = value;
    }

    private int __value;

    public override int GetValue()
    {
        return __value;
    }

    public override int GetValue<T>()
    {
        return 200;
    }

    public override int GetValue<T, K>()
    {
        return 2000;
    }
}


