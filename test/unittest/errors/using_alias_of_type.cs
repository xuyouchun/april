/* EXPECT_ERROR is a type: its alias is declared with typedef */

import System;

using System;
using Num = System.Int32;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine(1);
    }
}
