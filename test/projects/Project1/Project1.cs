
import System;

using System;
using System.Diagnostics;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
        MyClass obj = new MyClass(0);
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
    public MyClass(int a)
    {
        this.__A = a;
    }

    typedef System.Delegate<int, int, int> __Func;

    private int __A;

    [Trace]
    public void Execute()
    {
        int count = 0;
        for (int k = 2; k <= 100'0000; k += 1)
        {
            bool yes = true;
            for (int i = 2, end = (int)Math.Sqrt(k); i <= end; i++)
            {
                if (k % i == 0)
                {
                    yes = false;
                    break;
                }
            }

            if (yes)
                count++;
        }

        Console.WriteLine(count);
    }

    public int Add(int a, int b)
    {
        return a + b + __A;
    }

    public int Sub(int a, int b)
    {
        return a - b + __A;
    }

    public static __Func operator + (MyClass obj, int a)
    {
        return new MyClass(a).Add;
    }

    public static __Func operator - (MyClass obj, int a)
    {
        return new MyClass(-a).Sub;
    }

    private int __DoFunc(__Func func, int a, int b)
    {
        return func(a, b);
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
