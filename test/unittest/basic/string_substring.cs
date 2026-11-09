// String.Substring, written with the members of the string itself (Length).

/* EXPECT

world
hello
o w
empty:
done

*/

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        String s = "hello world";

        Console.WriteLine(s.Substring(6));
        Console.WriteLine(s.Substring(0, 5));
        Console.WriteLine(s.Substring(4, 3));
        Console.WriteLine("empty:" + s.Substring(11));
        Console.WriteLine("done");
    }
}
