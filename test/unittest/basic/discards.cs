// _ is a discard: it is no variable, nothing reads it. out _ , (_, _) => .. , (a, _) = tuple; , _ = call(); .

/* EXPECT

out 1
out value 7
lambda 5
lambda typed 4
assign 3
assign statement 1
calls 2

*/

import System;

using System;

class Program
{
    static Int32 Calls = 0;

    static Boolean Parse(String text, out Int32 value)
    {
        value = text.Length;
        return true;
    }

    static Int32 Count()
    {
        Calls++;
        return Calls;
    }

    [EntryPoint]
    public static void Main()
    {
        // out _
        Boolean ok = Parse("a", out _);
        Console.WriteLine("out " + (ok? 1 : 0));

        Boolean ok2 = Parse("1234567", out var value);
        Console.WriteLine("out value " + value);

        // The two params of the lambda are discards.
        Func<Int32, Int32, Int32> five = (_, _) => 5;
        Console.WriteLine("lambda " + five(1, 2));

        Func<Int32, Int32, Int32> typed = (Int32 _, Int32 _) => 4;
        Console.WriteLine("lambda typed " + typed(1, 2));

        // Deconstruction.
        Int32 a = 0;
        (a, _) = (3, 4);
        Console.WriteLine("assign " + a);

        // _ = expression; : the value is not used.
        _ = Count();
        Console.WriteLine("assign statement " + Calls);

        _ = Count();
        Console.WriteLine("calls " + Calls);
    }
}
