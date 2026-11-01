// The delegates that are local variables of an iterator are called, with lambdas in lambdas.

/* EXPECT

14
5
10
101
110

*/

import System;

using System;
using System.Collections.Generic;

class Program
{
    // A delegate that is a local variable of an iterator is called.
    static IEnumerable<Int32> Calls(Int32 k)
    {
        Delegate<Int32, Int32> twice = (Int32 x) => x * 2;
        Delegate<Int32> five = Five;
        yield return twice(k);
        yield return five();
        yield return twice(five());
    }

    static int Five()
    {
        return 5;
    }

    // The lambda in a method that has the iterator, with a lambda in a lambda.
    static IEnumerable<Int32> Nested(Int32 k)
    {
        Delegate<Delegate<Int32>, Int32> outer = (Int32 a) => () => a + k;
        yield return outer(1)();
        yield return outer(10)();
    }

    [EntryPoint]
    public static void Main()
    {
        foreach (var x in Calls(7))
            Console.WriteLine(x);

        foreach (var x in Nested(100))
            Console.WriteLine(x);
    }
}
