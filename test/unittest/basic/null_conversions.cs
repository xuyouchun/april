// null is converted to references and to T?.

/* EXPECT

True
True
True
True

*/

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        String s = null;
        int[] a = null;
        Object o = null;
        int? n = null;

        Console.WriteLine(s == null);
        Console.WriteLine(a == null);
        Console.WriteLine(o == null);
        Console.WriteLine(n == null);
    }
}
