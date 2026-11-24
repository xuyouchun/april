/* EXPECT_ERROR the type of "default" is not known here */
import System;
using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        var x = default;
    }
}
