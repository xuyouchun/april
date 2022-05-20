
// Instance call.

/* EXPECT

100
100

*/

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        // Ref arguments
        {
            int value = 100;
            __WithRefArgument(ref value);

            Console.WriteLine(value);
        }

        // Out arguments
        {
            int value = 100;
            __WithOutArgument(out value);

            Console.WriteLine(value);
        }

    }

    private static void __WithRefArgument(ref int value)
    {
        value = 100;
    }

    private static void __WithOutArgument(out int value)
    {
        value = 100;
    }

}
