/* EXPECT_ERROR member "Item1" is not defined in type */
/* EXPECT_CODE AP3206 */

// The items of a tuple that is written with names have the names only: Item1 is not the name of the item Number.

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        (Int32 Number, String Str) t = (1, "2");
        Console.WriteLine(t.Item1);
    }
}
