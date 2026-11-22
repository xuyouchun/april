/* EXPECT_ERROR a line of a raw string has less indentation than the closing quotes */

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine("""
            fine
          less
            """);
    }
}
