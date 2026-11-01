// checked { ... } and unchecked { ... } blocks, nested with checked(...) and unchecked(...): the arithmetic
// (+ - * ++ -- += -= *= and the minus) raises OverflowException when the result does not fit, or wraps.

/* EXPECT

block overflow
-2147483648
-2147483648
-2147483648
inner checked
++ overflow
-- overflow
+= overflow
*= overflow
minus overflow
-2147483648
long overflow
uint overflow
60000
-2147483648
2147483647
3000000001
*/
import System;
using System;

class Program
{
    static int Add(int a, int b) { return a + b; }

    [EntryPoint]
    public static void Main()
    {
        int big = 2147483647;
        int one = 1;

        // A checked block: every arithmetic of the statements raises on overflow.
        try { checked { int x = big + one; Console.WriteLine(x); } }
        catch (OverflowException) { Console.WriteLine("block overflow"); }

        // It ends with the block.
        Console.WriteLine(big + one);

        // Unchecked in checked, and the other way.
        checked {
            Console.WriteLine(unchecked(big + one));
            unchecked { Console.WriteLine(big + one); }
            try { Console.WriteLine(unchecked(checked(big + one))); }
            catch (OverflowException) { Console.WriteLine("inner checked"); }
        }

        // ++, --, +=, -=, *=, and the minus.
        int c = big;
        try { checked { c++; } Console.WriteLine(c); }
        catch (OverflowException) { Console.WriteLine("++ overflow"); }

        c = -big - 1;
        try { checked { c--; } Console.WriteLine(c); }
        catch (OverflowException) { Console.WriteLine("-- overflow"); }

        c = big;
        try { checked { c += 1; } Console.WriteLine(c); }
        catch (OverflowException) { Console.WriteLine("+= overflow"); }

        c = big;
        try { checked { c *= 2; } Console.WriteLine(c); }
        catch (OverflowException) { Console.WriteLine("*= overflow"); }

        int min = -big - 1;
        try { Console.WriteLine(checked(-min)); }
        catch (OverflowException) { Console.WriteLine("minus overflow"); }
        Console.WriteLine(-min);

        // Other types.
        long l = 9223372036854775807;
        try { checked { l = l + 1; } Console.WriteLine(l); }
        catch (OverflowException) { Console.WriteLine("long overflow"); }

        uint u = (uint)4294967295;
        try { Console.WriteLine(checked(u + (uint)1)); }
        catch (OverflowException) { Console.WriteLine("uint overflow"); }

        short s = (short)30000;
        Console.WriteLine(checked(s + s));

        // The arithmetic of a method that is called is not checked.
        checked { Console.WriteLine(Add(big, one)); }

        // Values that fit.
        checked { Console.WriteLine(big - one + one); }

        // Mixed with a variable of another type.
        long w = 3000000000;
        Console.WriteLine(checked(w + one));
    }
}
