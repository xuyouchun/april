// The lambdas in an iterator use its params and its local variables, which are fields of the state object.

/* EXPECT

111
122
133
10
12
14

*/

import System;

using System;
using System.Collections.Generic;

class Program
{
    // The lambda uses a param and a local variable of the iterator.
    static IEnumerable<Int32> Scaled(Int32 k, Int32 count)
    {
        for (int i = 1; i <= count; i++)
        {
            int n = i * 10;
            Delegate<Int32, Int32> f = x => x + k + n;
            yield return f(i);
        }
    }

    // The lambdas are the items.
    static IEnumerable<Delegate<Int32>> Makers(Int32 base_)
    {
        for (int i = 0; i < 3; i++)
        {
            int v = base_ + i;
            yield return () => v * 2;
        }
    }

    [EntryPoint]
    public static void Main()
    {
        foreach (var x in Scaled(100, 3))
            Console.WriteLine(x);

        foreach (var m in Makers(5))
            Console.WriteLine(m());
    }
}
