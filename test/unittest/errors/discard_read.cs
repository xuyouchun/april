/* EXPECT_ERROR variable "_" is not defined */
import System;
using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        var (_, b) = (1, 2);
        Console.WriteLine(_);
    }
}
