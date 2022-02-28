import System;

using System;
using System.Diagnostics;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
        /*
        BaseClass obj = new MyClass();
        int r = obj.GetValue();

        Console.WriteLine(r);
        */
    }
}

interface IMyInterface
{
    int GetValue { get; }
}

class BaseClass
{
    public virtual int GetValue(int b) { return 0; }
}

class MyClass : BaseClass
{
    /*
    public sealed override int GetValue(int b)
    {
        return 2;
    }
    */
}

class MyClass2 : MyClass
{
    public override long GetValue(int b)
    {
        return 2;
    }
}
