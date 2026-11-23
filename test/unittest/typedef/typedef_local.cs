// A typedef in a method: it is seen from its place to the end of the block, and it is the type that it names. The statement
// using expression; (using new Res();) is another one: an expression that is an assignment is not that.

/* EXPECT

2
7
alias 5
0
1
closed res

*/

import System;

using System;
using System.Collections;

class Res : IDisposable
{
    public void Dispose() { Console.WriteLine("closed res"); }
}

class Program
{
    // The aliases of two methods have the same name and are not the same alias.
    static void CountInts()
    {
        typedef Items = List<Int32>;
        Items items = new Items();
        Console.WriteLine(items.Count);
    }

    static void CountNames()
    {
        typedef Items = List<String>;
        Items items = new Items();
        items.Add("x");
        Console.WriteLine(items.Count);
    }

    [EntryPoint]
    public static void Main()
    {
        typedef Items = List<Int32>;
        typedef Num = Int32;

        Items items = new Items();
        items.Add(1);
        items.Add(2);
        Console.WriteLine(items.Count);

        Num seven = 7;
        Console.WriteLine(seven);

        {
            typedef Pair<T> = Dictionary<T, T>;
            Pair<Int32> p = new Pair<Int32>();
            p[1] = 5;
            Console.WriteLine("alias " + p[1]);
        }

        CountInts();
        CountNames();

        using new Res();
    }
}
