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
        // Struct1 obj = new Struct1(103);
        // __Call1(obj);

        __Call1(new Struct1(104));
    }

    [Trace]
    private static void __Call1(Struct1 obj1)
    {
        Console.WriteLine("Hello Call1");
        Console.WriteLine(obj1.Value);        
    }
}

class Class1
{
    // public static void Call<T>(T obj)
    // {
    //     T obj1 = obj;
    // }

    // [Trace]
    public static void Call2<T1>(T1 obj1)
    {
        Call3(obj1);
    }

    // [Trace]
    public static void Call3<T2>(T2 obj)
    {
        T2 obj2 = obj;
    }

    // [Trace]
    public static T Call4<T>(T obj)
    {
        return obj;
    }
}

class Class2
{
    public int A, B, C;
}

struct Struct1
{
    [Trace]
    public Struct1(int value)
    {
        Value = value;
        // Console.WriteLine("Hello Struct1");
    }

    public int Value;
}


