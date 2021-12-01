
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

    public int Add(int a, int b)
    {
        return a + b;
    }
};

class MyClass
{
    typedef System.Delegate<int, int, int> __Func;

    public void Execute()
    {
        __Func func = GetFunc();
        int r = func(1, 2);
        Console.Write(r);
    }

    public int Add(int a, int b)
    {
        return a + b;
    }

    public int Sub(int a, int b)
    {
        return a - b;
    }

    public __Func GetFunc()
    {
        var func = this.Sub;
        return func;
    }

    public int DoFunc(__Func func, int a, int b)
    {
        return func(a, b);
    }
};

class MyClass<T1>
{
    class Nest<T2>
    {
        public static int Add(T1 a, T2 b)
        {
            return 0;
        }

        public static int Add2<T>(T a, T b)
        {
            return 0;
        }
    }
};

