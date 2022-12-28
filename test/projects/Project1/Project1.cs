import System;

using System;
using System.Diagnostics;

using EventHandle = System.Delegate<void, int, int>;

class Project1
{
    [EntryPoint]
    [Trace]
    public static void Main()
    {
        // Struct1 obj = new Struct1();
        Class2 obj = new Class2();

        Class1.Call2(obj);
    }
}


class Class1
{
    // public static void Call<T>(T obj)
    // {
    //     T obj1 = obj;
    // }

    [Trace]
    public static void Call2<T11>(T11 obj)
    {
        T11 obj1 = obj;
    }
}

class Class2
{
    public int A, B, C;
}

struct Struct1
{
    public int Value;
}


