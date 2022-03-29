import System;

using System;
using System.Diagnostics;

class Project1
{
    [EntryPoint, Trace]
    public static void Main()
    {
        IMyInterface obj1 = new MyClass();
        IMyInterface2 obj2 = obj1;

        int r = obj1.GetValue1() + obj2.GetMyValue();

        Console.WriteLine(r);
    }
}

interface IMyInterfaceBase
{
    int GetValue();
}

interface IMyInterface2
{
    int GetMyValue();
}

interface IMyInterface : IMyInterfaceBase
{
    int GetValue1();
    int GetValue2();
    int GetValue3();
}

class MyClass : IMyInterface, IMyInterface2
{
    public int GetValue()  { return 0; }
    public int GetValue1() { return 1; }
    public int GetValue2() { return 2; }
    public int GetValue3() { return 3; }

    public int GetMyValue() { return 100; }
}

