// A struct that is made or returned and not used: the statement new S(1, 2); and the calls that return a struct.

/* EXPECT

made ok
calls ok 2
loop ok 3
try ok
indexer 3 3
default ok
after 1

*/

import System;

using System;

struct S
{
    public S(Int32 a, Int32 b)
    {
        A = a;
        B = b;
    }

    public Int32 A;

    public Int32 B;

    public S Cut(Int32 d) => new S(A, B - d);

    public static S Make(Int32 v) => new S(v, v);

    public Int32 this[Int32 i] => A + i;
}

struct Z
{
    public Int32 V;
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        new S(1, 2);
        new Z();
        Console.WriteLine("made ok");

        S s = new S(1, 2);
        s.Cut(1);
        s.Cut(2);
        S.Make(3);
        Console.WriteLine("calls ok " + s.B);

        Int32 count = 0;
        for (Int32 k = 0; k < 3; k++)
        {
            s.Cut(k);
            count++;
        }

        Console.WriteLine("loop ok " + count);

        try { s.Cut(1); } catch (OutOfRangeException) { }
        try { new S(1, 2); } catch (OutOfRangeException) { }
        Console.WriteLine("try ok");

        Console.WriteLine("indexer " + s[2] + " " + s.Cut(1)[2]);

        default(S);
        Console.WriteLine("default ok");

        // The variables keep their values.
        S t = s.Cut(1);
        Console.WriteLine("after " + t.B);
    }
}
