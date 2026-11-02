// The patterns of is: is null, is not null, is not T, with the other operators.

/* EXPECT

False
True
True
False
True
False
ok
ok2
two
5

*/

import System;

using System;

class B
{
    public int N;
}

class D : B
{
}

class Program
{
    static String Kind(Object o)
    {
        if (o is not D)
            return "other";

        return "d";
    }

    [EntryPoint]
    public static void Main()
    {
        B b = new B();
        b.N = 3;
        String s = null;

        Console.WriteLine(b is null);
        Console.WriteLine(b is not null);
        Console.WriteLine(s is null);
        Console.WriteLine(b is not B);
        Console.WriteLine(b is B);
        Console.WriteLine(s is not null);

        // is has the precedence of the comparison: it is not the operand of &&.
        if (b is not null && b.N == 3)
            Console.WriteLine("ok");

        if (b is B && b.N == 3)
            Console.WriteLine("ok2");

        Console.WriteLine(Kind(new D()) == "d"? "two" : "no");

        B c = b as B;
        Console.WriteLine(c is null? 0 : 5);
    }
}
