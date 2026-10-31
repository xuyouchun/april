// Generic types and methods.

/* EXPECT

5
9
5000000000
True

42
7
7
8

*/

import System;

using System;

class Box<T>
{
    public Box(T value)
    {
        Value = value;
    }

    public T Value;

    public void Set(T value)
    {
        Value = value;
    }

    public Box<T> Copy()
    {
        return new Box<T>(Value);
    }
}

struct Pair<T1, T2>
{
    public T1 First;
    public T2 Second;
}

class Program
{
    static T Identity<T>(T value)
    {
        return value;
    }

    static T Pick<T>(bool first, T a, T b)
    {
        if (first)
            return a;

        return b;
    }

    [EntryPoint]
    public static void Main()
    {
        // A generic class with different arguments.
        {
            Box<int> a = new Box<int>(5);
            Console.WriteLine(a.Value);

            a.Set(9);
            Console.WriteLine(a.Value);

            Box<long> b = new Box<long>(5000000000);
            Console.WriteLine(b.Value);

            Box<bool> c = new Box<bool>(true);
            Console.WriteLine(c.Value);
        }

        // Nested.
        {
            Box<Box<int>> outer = new Box<Box<int>>(new Box<int>(42));
            Console.WriteLine(outer.Value.Value);
            Box<Box<int>> clone = outer.Copy();
            clone.Value.Set(7);
            Console.WriteLine(outer.Value.Value);
        }

        // A generic struct.
        {
            Pair<int, long> p;
            p.First = 7;
            p.Second = 8;
            Console.WriteLine(p.First);
            Console.WriteLine(p.Second);
        }
    }
}
