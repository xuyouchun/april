/* EXPECT_ERROR (unmanaged) */

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Ptr<String> p = Ptr<String>.Null;
        Console.WriteLine(p.IsNull);
    }
}
