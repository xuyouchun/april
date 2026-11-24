/* EXPECT_ERROR a throw expression is the right operand of \?\?, or a branch of \?: */
import System;
using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Int32 k = 1;
        var x = k > 0? throw new Exception("a") : default;
    }
}
