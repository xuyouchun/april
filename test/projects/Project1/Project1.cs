
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
        var func1 = this.Add, func2 = this.Sub;
        var r = func1(2, 3) * func2(4, 5);

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

    private MyClass2 __GetObject2()
    {   
        return new MyClass2(__A, __B);
    }
};

class MyClass2
{
    public MyClass2(int a, int b)
    {
        this.__A = a;
        this.__B = b;
    }

    private int __A, __B;

    public int Add(int a, int b)
    {
        return a + b + __A * __B;
    }
};
