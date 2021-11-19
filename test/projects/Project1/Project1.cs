
import System;

using System;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
        /*
        int a = 10, b = 20;
        int c = a + b;

        Console.WriteLine(c);
        */

        // var func1 = MyClass3.Nest.Add;

        var func2 = MyClass<int>.Nest<long>.Add;
    }
};

struct MyStruct
{
    public MyStruct(int a)
    {
        A = a;
    }

    public int A;
};

class MyClass<T1>
{
    class Nest<T2>
    {
        public static int Add(T1 a, T2 b)
        {
            return 0;
        }
    }
};

class MyClass
{
    public MyClass(int a)
    {
        this.Obj2 = new MyClass2();
    }

    public int A;

    public static int Add<T>(T a, T b)
    {
        return 0;
    }

    public MyClass2 Obj2;
    public static int Value = 100;
};

class MyClass2
{
    public int A = 20;
};

class MyClass3
{
    class Nest
    {
        public static int Add(int a, int b)
        {
            return a + b;
        }
    };
};


