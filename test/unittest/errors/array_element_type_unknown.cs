/* EXPECT_ERROR type of the elements of the array cannot be known */
import System;
using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        var items = new[] { 1, "a" };
    }
}
