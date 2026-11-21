// A | B, A & B and A ^ B of two values of the same enum are a value of that enum: they are passed and returned as
// the enum, and compared with it (the flags).

/* EXPECT

5
2
True
True
6
7
7
3 False
mixed 7

*/

import System;

using System;

enum Opt : Int32
{
    None = 0, I = 1, M = 2, S = 4;
}

class Program
{
    static Int32 F(Opt o) { return (Int32)o; }

    static Opt Both(Opt a, Opt b) { return a | b; }

    static Boolean Has(Opt value, Opt flag) { return (value & flag) == flag; }

    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine(F(Opt.I | Opt.S));
        Opt x = Opt.I | Opt.M | Opt.S;
        Console.WriteLine(F(x & Opt.M));
        Console.WriteLine((x & Opt.M) != Opt.None);
        Console.WriteLine((x & Opt.M) == Opt.M);
        Console.WriteLine(F(x ^ Opt.I));
        x |= Opt.None;
        Console.WriteLine(F(x));
        Console.WriteLine(F((Opt)3 | Opt.S));
        Console.WriteLine(F(Both(Opt.I, Opt.M)) + " " + Has(Opt.I, Opt.M));

        // An enum and a number is still a number.
        Int32 n = (Int32)Opt.I | 6;
        Console.WriteLine("mixed " + n);
    }
}
