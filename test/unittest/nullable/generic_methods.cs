// EXPECT_WARNING 4010 4010 4006 4006 4006 4006

/* EXPECT

*/

#nullable enable

import System;
using System;
using System.Collections.Generic;

class Base { }

class Derived : Base { }

// Base cannot be null, Base? can be.
class NeedBase<T> where T : Base { }

class MaybeBase<T> where T : Base? { }

class Program
{
    static T Id<T>(T x) { return x; }

    static T FirstOf<T>(List<T> items) { return items[0]; }

    static T? Maybe<T>(T x) { return x; }

    static void Take(String s) { }

    // Never called.
    static void Cases()
    {
        List<String?> maybe_list = new List<String?>();
        String? m = null;
        String p = "a";

        Take(Id<String?>(null)!);                       // forgiving: ok
        Console.WriteLine(Id<String?>(null).Length);    // 4006: the param of the method is given
        Console.WriteLine(Id<String>("a").Length);      // ok

        Console.WriteLine(Id(m).Length);                // 4006: the param is inferred from the argument
        Console.WriteLine(Id(p).Length);                // ok

        Console.WriteLine(FirstOf<String?>(maybe_list).Length);  // 4006

        Console.WriteLine(Maybe("x").Length);           // 4006: T? can be null

        var r = Id(m);
        Console.WriteLine(r!.Length);                   // forgiving: ok

        NeedBase<Derived?> a = new NeedBase<Derived?>();        // 4010 x 2: Base cannot be null
        MaybeBase<Derived?> b = new MaybeBase<Derived?>();      // ok
        NeedBase<Derived> c = new NeedBase<Derived>();          // ok
    }

    [EntryPoint]
    public static void Main()
    {
    }
}
