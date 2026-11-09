/* EXPECT_ERROR cannot be indexed by 3 indices, expected 1 or 2 */
/* EXPECT_CODE AP5310 */

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        // An array is read by one index (the position), or by one index for each dimension.
        Int32[,] m = new Int32[2, 2];
        Console.WriteLine(m[0, 0, 0]);
    }
}
