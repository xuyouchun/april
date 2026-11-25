// The operators of the nullable numbers: a + b of Int32? and Int32. The result has no value when an operand has none; a
// comparison is false then (two operands that have no value are equal).

/* EXPECT

add 5
add none True
sub 1
mul 6
div 1
mod 1
mixed 7
both 9
compare True False False
equal True False True
bits 1 7 6 0
shift 6 1
neg -3
compound 6
chain 11
long 30
char 98 True

*/

import System;

using System;

class Program
{
    static String Show(Int32? n) { return n.HasValue? "" + n.Value : "none"; }

    [EntryPoint]
    public static void Main()
    {
        Int32? a = 3;
        Int32? b = 2;
        Int32? none = null;

        Console.WriteLine("add " + Show(a + b));
        Console.WriteLine("add none " + ((a + none).HasValue == false));
        Console.WriteLine("sub " + Show(a - 2));
        Console.WriteLine("mul " + Show(a * b));
        Console.WriteLine("div " + Show(a / 1 / b));
        Console.WriteLine("mod " + Show(a % b));
        Console.WriteLine("mixed " + Show(4 + a));
        Console.WriteLine("both " + Show((a + b) * 2 - 1));

        Console.WriteLine("compare " + (a > b) + " " + (a < none) + " " + (none >= a));
        Console.WriteLine("equal " + (a == 3) + " " + (a == none) + " " + (none == null));

        Console.WriteLine("bits " + Show(a & 1) + " " + Show(a | 4) + " " + Show(a ^ 5) + " " + Show(a & 4));
        Console.WriteLine("shift " + Show(a << 1 + 0 + 0 - 0 + 0) + " " + Show(a >> 1 + 0));

        Console.WriteLine("neg " + Show(-a));

        Int32? total = 1;
        total += 5;
        Console.WriteLine("compound " + Show(total));
        total = total + a + 2;
        Console.WriteLine("chain " + Show(total));

        Int64? big = 10;
        Console.WriteLine("long " + Show2(big * 3));

        // A Char is converted to Int32.
        Char c = 'a';
        Int32 code = c + 1;
        Console.WriteLine("char " + code + " " + (c < 'b'));
    }

    static String Show2(Int64? n) { return n.HasValue? "" + n.Value : "none"; }
}
