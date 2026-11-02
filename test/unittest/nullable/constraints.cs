// EXPECT_WARNING 4010 4010 4010 4010

/* EXPECT

*/

#nullable enable

import System;

using System;

class Node
{
}

// The argument cannot be null.
class NotNull<T> where T : notnull
{
    public T Value;

    public NotNull(T value)
    {
        Value = value;
    }
}

// A class that cannot be null, and a class that can be.
class Cls<T> where T : class
{
    public T? Value;
}

class Opt<T> where T : class?
{
    public T? Value;
}

class Free<T>
{
    public T? Value;
}

class Program
{
    // Never called.
    static void Cases()
    {
        NotNull<String> a = new NotNull<String>("x");           // ok
        NotNull<String?> b = new NotNull<String?>(null);        // 4010 x 2
        Cls<Node?> c = new Cls<Node?>();                        // 4010 x 2
        Cls<Node> d = new Cls<Node>();                          // ok
        Opt<Node?> e = new Opt<Node?>();                        // ok: class?
        Free<String?> f = new Free<String?>();                  // ok
    }

    [EntryPoint]
    public static void Main()
    {
    }
}
