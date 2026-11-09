// A generic type calls its own static methods by their simple names, in an instance method and in a static one, in a
// class and in a struct.

/* EXPECT

struct 11 7 abc
class 11 12 9
qualified 11
nested 9 10

*/

import System;

using System;

struct Box<T>
{
    public Box(T value)
    {
        __value = value;
    }

    public static Int32 Twice(Int32 n) => n * 2;

    public static T Same(T value) => value;

    public Int32 Run(Int32 n) => Twice(n) + 1;

    public T Pick(T value) => Same(value);

    public Int32 RunQualified(Int32 n) => Box<T>.Twice(n) + 1;

    private T __value;
}

class Counter<T>
{
    public static Int32 Twice(Int32 n) => n * 2;

    public static Int32 Both(Int32 n) => Twice(n) + Twice(1);

    public Int32 Run(Int32 n) => Twice(n) + 1;

    public static Int32 Nested(Int32 n) => Both(n) - Twice(n) + n;
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Box<Int32> number = new Box<Int32>(1);
        Box<String> text = new Box<String>("x");
        Console.WriteLine("struct " + number.Run(5) + " " + number.Pick(7) + " " + text.Pick("abc"));

        Counter<Int32> counter = new Counter<Int32>();
        Console.WriteLine("class " + counter.Run(5) + " " + Counter<Int32>.Both(5) + " " + Counter<String>.Nested(7));

        Console.WriteLine("qualified " + number.RunQualified(5));
        Console.WriteLine("nested " + Counter<Int32>.Nested(7) + " " + Counter<Int32>.Nested(8));
    }
}
