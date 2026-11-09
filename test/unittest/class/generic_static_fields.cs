// A generic type has a copy of its static fields for each of its instances.

/* EXPECT

int 2
bool 1
text 1
done

*/

import System;

using System;

class Box<T>
{
    public static Int32 Count;
    public T Value;

    public Box(T value)
    {
        Value = value;
        Count = Count + 1;
    }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Box<Int32> a = new Box<Int32>(1);
        Box<Int32> b = new Box<Int32>(2);
        Box<Boolean> c = new Box<Boolean>(true);
        Box<String> d = new Box<String>("x");

        Console.WriteLine("int " + Box<Int32>.Count);
        Console.WriteLine("bool " + Box<Boolean>.Count);
        Console.WriteLine("text " + Box<String>.Count);
        Console.WriteLine("done");
    }
}
