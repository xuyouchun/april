import System;

using System;
using System.Diagnostics;

class Project1
{
    [EntryPoint, Trace]
    public static void Main()
    {
        MyClass obj = new MyClass();
        int r = obj.GetValue1();
    }
}

interface IMyInterfaceBase
{
    int GetValue();
}

interface IMyInterface : IMyInterfaceBase
{
    int GetValue1();
    int GetValue2();
    int GetValue3();
}

class MyClass : IMyInterface
{
    public int GetValue()  { return 0; }
    public int GetValue1() { return 1; }
    public int GetValue2() { return 2; }
    public int GetValue3() { return 3; }
}

