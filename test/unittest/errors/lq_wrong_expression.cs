/* EXPECT_ERROR variable "missing" is not defined */

import System;
using System.Linq;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Int32[] xs = new Int32[] { 5, 1 };
        var a = ``lq from x in xs
                  where x > 1
                  where missing > 2
                  select x;
    }
}
