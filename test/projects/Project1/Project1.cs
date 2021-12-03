
import System;

using System;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
        MyClass obj = new MyClass();
        obj.Execute();
    }
};

class MyClass
{
    typedef System.Delegate<int, int, int> __Func;

    public void Execute()
    {
        Console.WriteLine( __DoFunc(this.Sub, 10, 20) );
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

struct MyStruct
{
    public MyStruct(int a, int b)
    {
        A = a;
        B = b;
        C = 1;
        D = 2;
    }

    public int A, B, C, D;
};

