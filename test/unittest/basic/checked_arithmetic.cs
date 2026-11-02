// checked(expression): the arithmetic raises OverflowException when the result does not fit.

/* EXPECT

-2147483648
2147483646
overflow
overflow64
-2147483648
overflow unsigned
6
overflow
7
-1

*/

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        int big = 2147483647;
        int one = 1;

        // Not checked: wraps.
        Console.WriteLine(big + one);

        Console.WriteLine(checked(big - one));

        try
        {
            Console.WriteLine(checked(big + one));
        }
        catch (OverflowException)
        {
            Console.WriteLine("overflow");
        }

        long l = 9223372036854775807;
        try
        {
            Console.WriteLine(checked(l * 2));
        }
        catch (OverflowException)
        {
            Console.WriteLine("overflow64");
        }

        Console.WriteLine(unchecked(big + one));

        uint u = (uint)3;
        uint v = (uint)5;
        try
        {
            Console.WriteLine(checked(u - v));
        }
        catch (OverflowException)
        {
            Console.WriteLine("overflow unsigned");
        }

        // Checked in an expression, with variables and assignment.
        int a = 2, b = 3;
        Console.WriteLine(checked(a * b));

        int c = big;
        try
        {
            c = checked(c + 2);
            Console.WriteLine(c);
        }
        catch (OverflowException)
        {
            Console.WriteLine("overflow");
        }

        // The type of the result is that of the expression.
        long wide = checked(l - 9223372036854775800);
        Console.WriteLine(wide);

        Console.WriteLine(checked(a - b));
    }
}
