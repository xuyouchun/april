// A loop whose body always returns can end the normal way: when its condition is false at once
// (a foreach of a collection that has no item), the code that follows it is run, and the
// resources of the loop (the enumerator of a foreach) are released.

/* EXPECT

True
5
-1
False
while 0
for 0
while 1
for 1
no unreachable code: 7

*/

import System;

using System;
using System.Collections;

class Program
{
    static Boolean AnyOf(List<Int32> items)
    {
        foreach (Int32 item in items)
            return true;

        return false;
    }

    static Int32 FirstOf(List<Int32> items)
    {
        foreach (Int32 item in items)
        {
            return item;
        }

        return -1;
    }

    static Int32 WhileOf(Int32 count)
    {
        Int32 i = 0;
        while (i < count)
            return 1;

        return 0;
    }

    static Int32 ForOf(Int32 count)
    {
        for (Int32 i = 0; i < count; i++)
            return 1;

        return 0;
    }

    [EntryPoint]
    public static void Main()
    {
        List<Int32> empty = new List<Int32>();
        List<Int32> one = new List<Int32>();
        one.Add(5);

        Console.WriteLine(AnyOf(one));
        Console.WriteLine(FirstOf(one));
        Console.WriteLine(FirstOf(empty));
        Console.WriteLine(AnyOf(empty));

        Console.WriteLine("while " + WhileOf(0));
        Console.WriteLine("for " + ForOf(0));
        Console.WriteLine("while " + WhileOf(3));
        Console.WriteLine("for " + ForOf(3));

        Console.WriteLine("no unreachable code: " + (FirstOf(one) + 2));
    }
}
