/* EXPECT_ERROR type "Callback" is not defined */

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Int32 n = 0;
        Callback a = () => { n++; };
        a();
        Console.WriteLine(n);
    }
}
