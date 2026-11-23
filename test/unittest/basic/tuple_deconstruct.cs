// Deconstruction: (Int32 Number, String Str) = tuple; declares the variables Number and Str, of the items of the tuple (a
// tuple literal, or any tuple: the types of the items are converted to those of the variables, var tells them);
// (a, b) = (b, a); assigns the variables that exist, the value is made before the first one is assigned.

/* EXPECT

12
3four
big 3 four
15
2 1
3 1 2
7 x
swap 4 3
ref 5 six

*/

import System;

using System;

typedef MyTuple = (Int32 Number, String Str);

class Program
{
    static (Int32, String) Make() { return (7, "x"); }

    [EntryPoint]
    public static void Main()
    {
        (Int32 Number, String Str) = (1, "2");
        Console.WriteLine(Number + Str);

        MyTuple tuple1 = (3, "four");
        (Int64 big, var text) = tuple1;
        Console.WriteLine(big + text);
        Console.WriteLine("big " + tuple1.Number + " " + tuple1.Str);

        (var x, var y) = (7, 8);
        Console.WriteLine(x + y);

        Int32 a = 1, b = 2;
        (a, b) = (b, a);
        Console.WriteLine(a + " " + b);

        Int32 p = 1, q = 2, r = 3;
        (p, q, r) = (r, p, q);
        Console.WriteLine(p + " " + q + " " + r);

        (Int32 n, String s) = Make();
        Console.WriteLine(n + " " + s);

        Int32 left = 3, right = 4;
        (left, right) = (right, left);
        Console.WriteLine("swap " + left + " " + right);

        ITuple<Int32, String> boxed = (5, "six");
        (Int32 id, String name) = boxed;
        Console.WriteLine("ref " + id + " " + name);
    }
}
