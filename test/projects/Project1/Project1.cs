
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
    public void Execute()
    {
        var func = this.Add;
    }

    public int Add(int a, int b)
    {
        return a + b;
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

