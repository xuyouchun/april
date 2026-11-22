// A yield in a try block: the finally block runs when the block is left (not at each yield), and a catch handler works after the
// iterator went on.

/* EXPECT

1
2
fin
3
0
1
caught boom
2

*/

import System;

using System;
using System.Collections;

class Program
{
    static IEnumerable<Int32> Gen()
    {
        try
        {
            yield return 1;
            yield return 2;
        }
        finally
        {
            Console.WriteLine("fin");
        }

        yield return 3;
    }

    static IEnumerable<Int32> Gen2()
    {
        for (Int32 i = 0; i < 3; i++)
        {
            try
            {
                yield return i;
                if (i == 1)
                    throw new Exception("boom");
            }
            catch (Exception e)
            {
                Console.WriteLine("caught " + e.Message);
            }
        }
    }

    [EntryPoint]
    public static void Main()
    {
        foreach (Int32 x in Gen())
            Console.WriteLine(x);

        foreach (Int32 x in Gen2())
            Console.WriteLine(x);
    }
}
