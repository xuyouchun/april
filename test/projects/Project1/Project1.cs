
import System;

using System;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
        MyClass obj = new MyClass(10, 20);
        obj.Execute();
    }
};

class BaseClass
{
    public int Mul(int a, int b)
    {
        return a * b;
    }
};

class MyClass : BaseClass
{
    public MyClass(int a, int b)
    {
        this.__A = a;
        this.__B = b;
    }

    typedef System.Delegate<int, int, int> __Func;

    private int __A, __B;

    public void Execute()
    {
        var func = base.Mul;
        int r = func(2, 3);

        Console.WriteLine(r);
    }

    public int Add(int a, int b)
    {
        return a + b;
    }

    public int Sub(int a, int b)
    {
        return a - b;
    }

    private int __DoFunc(__Func func, int a, int b)
    {
        return func(a, b);
    }
};

