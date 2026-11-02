/* EXPECT_ERROR the local variable "i" is used by a lambda, it cannot be modified */

import System;

using System;
using System.Collections.Generic;

class Program
{
    static IEnumerable<Int32> Bad(Int32 count)
    {
        for (int i = 0; i < count; i++)
        {
            Delegate<Int32> f = () => i;
            yield return f();
        }
    }

    [EntryPoint]
    public static void Main()
    {
        foreach (var x in Bad(2))
            Console.WriteLine(x);
    }
}
