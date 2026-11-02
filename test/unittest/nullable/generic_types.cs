// EXPECT_WARNING 4006 4006 4006 4006 4008 4005

/* EXPECT

*/

#nullable enable

import System;
using System;

// T? can be null, whatever T is; T is null when the argument is annotated.
class Box<T>
{
    public T? Optional;
    public T Value;

    public Box(T value)
    {
        Value = value;
    }

    public T? Find() { return Optional; }

    public T Get() { return Value; }

    public void Set(T value) { Value = value; }
}

class Program
{
    static void Take(String s) { }

    static T? First<T>(T[] items)
    {
        if (items.Length == 0)
            return default(T);

        return items[0];
    }

    // Never called.
    static void Cases()
    {
        Box<String> plain = new Box<String>("a");
        Box<String?> maybe = new Box<String?>(null);

        Console.WriteLine(plain.Get().Length);      // not null
        Console.WriteLine(plain.Value.Length);      // not null
        Console.WriteLine(plain.Optional.Length);   // 4006: T? can be null
        Console.WriteLine(plain.Find().Length);     // 4006: T? can be null
        Console.WriteLine(maybe.Get().Length);      // 4006: the argument is annotated
        Console.WriteLine(maybe.Value.Length);      // 4006: the argument is annotated

        // The var of a new object has the arguments.
        var v = new Box<String?>(null);
        Take(v.Get());                              // 4008

        plain.Set(null);                            // 4005: the argument is not annotated
        maybe.Set(null);                            // the argument is annotated: no warning
    }

    [EntryPoint]
    public static void Main()
    {
    }
}
