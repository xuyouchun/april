
import System;

using System;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
        // var func = MyClass<int>.Nest<long>.Add;

        var func = MyClass.Add<int>;
    }
};

class MyClass
{
    public static int Add<T>(T a, T b)
    {
        return 0;
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
    }
};

