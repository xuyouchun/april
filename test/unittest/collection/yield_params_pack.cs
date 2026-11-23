// The iterators that are generic methods with a params generic parameter (T ... items): the state
// object keeps the items, the code of the iterator gives them to other methods (items ...).

/* EXPECT

count 3
head k
count 3
tail k!
none 0
mixed 2 x
pack 4
inst 10 2
again count 3

*/

import System;

using System;
using System.Collections;

public static class Tools
{
    // How many items.
    public static Int32 Count<T ...>(T ... items)
    {
        return ITuple.Create(items ...).GetSize();
    }

    // A fixed param and the items; the items are given to a method.
    public static IEnumerable<String> Describe<T ...>(String key, T ... items)
    {
        yield return "count " + Count(items ...);
        yield return "head " + key;
        yield return "count " + Count(items ...);
        yield return "tail " + key + "!";
    }

    // No fixed params.
    public static IEnumerable<String> Only<T ...>(T ... items)
    {
        yield return "none " + Count(items ...);
    }

    // A generic param before the items, and a local variable.
    public static IEnumerable<String> Mixed<K, T ...>(K key, T ... items)
    {
        Int32 n = Count(items ...);
        yield return "mixed " + n + " " + key;
    }

    // A lot of items, the iterator is used again.
    public static IEnumerable<Int32> Pack<T ...>(T ... items)
    {
        for (Int32 k = 0; k < Count(items ...); k++)
            yield return Count(items ...);
    }
}

public class Host
{
    public Int32 Base;

    public Host(Int32 b) { this.Base = b; }

    // An instance method.
    public IEnumerable<String> Inst<T ...>(T ... items)
    {
        yield return "inst " + (Base + Tools.Count(items ...)) + " " + Tools.Count(items ...);
    }
}

public class Program
{
    [EntryPoint]
    public static void Main()
    {
        foreach (String s in Tools.Describe("k", 1, "two", 3.5))
            Console.WriteLine(s);

        foreach (String s in Tools.Only())
            Console.WriteLine(s);

        foreach (String s in Tools.Mixed("x", true, 7))
            Console.WriteLine(s);

        Int32 total = 0;
        foreach (Int32 v in Tools.Pack(1, 2, 3, 4))
            total++;

        Console.WriteLine("pack " + total);

        foreach (String s in new Host(8).Inst("a", 2))
            Console.WriteLine(s);

        // The same iterator is made again, with other items, and left before its end.
        IEnumerator<String> again = Tools.Describe("z", 1, 2, 3).GetEnumerator();
        again.MoveNext();
        Console.WriteLine("again " + again.Current);
    }
}
