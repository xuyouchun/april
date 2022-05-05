import System;

using System;
using System.Diagnostics;

/*
class Project1
{
    [EntryPoint]
    public static void Main()
    {
        Class1 obj = new Class1(100);
        int r = obj.Add(2, 3);

        Console.WriteLine(r);
    }

    private static int __Add(int a, int b)
    {
        return a + b;
    }
}

class Class1
{
    public Class1(int a)
    {
        __a = a;
    }

    private int __a;

    public int Add(int b, int c)
    {
        return __a + b + c;
    }
}
*/

// A large tuple type with 100 elements.
using LargeTuple = Tuple<
    int, int, int, int, int, int, int, int, int, int,
    int, int, int, int, int, int, int, int, int, int,
    int, int, int, int, int, int, int, int, int, int,
    int, int, int, int, int, int, int, int, int, int,
    int, int, int, int, int, int, int, int, int, int,
    int, int, int, int, int, int, int, int, int, int,
    int, int, int, int, int, int, int, int, int, int,
    int, int, int, int, int, int, int, int, int, int,
    int, int, int, int, int, int, int, int, int, int,
    int, int, int, int, int, int, int, int, int, int
>;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
        int k = 1;
        var obj = new LargeTuple(
            k++, k++, k++, k++, k++, k++, k++, k++, k++, k++,
            k++, k++, k++, k++, k++, k++, k++, k++, k++, k++,
            k++, k++, k++, k++, k++, k++, k++, k++, k++, k++,
            k++, k++, k++, k++, k++, k++, k++, k++, k++, k++,
            k++, k++, k++, k++, k++, k++, k++, k++, k++, k++,
            k++, k++, k++, k++, k++, k++, k++, k++, k++, k++,
            k++, k++, k++, k++, k++, k++, k++, k++, k++, k++,
            k++, k++, k++, k++, k++, k++, k++, k++, k++, k++,
            k++, k++, k++, k++, k++, k++, k++, k++, k++, k++,
            k++, k++, k++, k++, k++, k++, k++, k++, k++, k++
        );

        // var r = obj.Item98;
        // Console.WriteLine(r);

        __DoSomething(obj);
    }

    [Trace]
    private static void __DoSomething(LargeTuple tuple)
    {
        var r = tuple.Item26;
        // Console.WriteLine(r);
    }
}

struct Struct1
{
    public Struct1(int value1, int value2)
    {
        Value1 = value1;
        Value2 = value2;
    }

    public int Value1;

    public int Value2;
}

class Class1<T>
{
    public Class1(T value)
    {
        Value = value;
    }

    public T Value;
}



