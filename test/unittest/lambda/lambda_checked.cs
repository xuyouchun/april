// The lambdas are checked or unchecked like the place where they are: checked { ... }, checked(...).

/* EXPECT

plain cast=-1294967296
plain add=-2147483648
block cast overflow
block add overflow
block block lambda overflow
unchecked expression=-2147483648
unchecked block=-2147483648
nested overflow
captured overflow
overflow
-2147483648
after=-2147483648
42

*/

import System;

using System;

class Program
{
    static int Max = 2147483647;
    static int One = 1;

    static void Try(String name, Delegate<Int32> f)
    {
        try
        {
            Console.WriteLine(name + "=" + f());
        }
        catch (OverflowException)
        {
            Console.WriteLine(name + " overflow");
        }
    }

    static int Run(Delegate<Int32> f)
    {
        return f();
    }

    static String Safe(Delegate<Int32> f)
    {
        try
        {
            return "" + f();
        }
        catch (OverflowException)
        {
            return "overflow";
        }
    }

    static Delegate<Int32, Int32> Make(Int32 k)
    {
        return x => x + k;
    }

    [EntryPoint]
    public static void Main()
    {
        long big = 3000000000;
        int max = 2147483647;
        int one = 1;

        // Not checked.
        Try("plain cast", () => (int)big);
        Try("plain add", () => max + one);

        // In a checked block: the lambda is checked.
        checked
        {
            Try("block cast", () => (int)big);
            Try("block add", () => max + one);
            Try("block block lambda", () =>
            {
                int r = max * 2;
                return r;
            });

            // unchecked in checked.
            Try("unchecked expression", () => unchecked(max + one));
            unchecked
            {
                Try("unchecked block", () => max + one);
            }

            // A lambda in a lambda.
            Delegate<Delegate<Int32>> outer = () => () => Max + One;
            Try("nested", outer());

            // A lambda that uses variables.
            Delegate<Int32> captured = () => max + one;
            Try("captured", captured);
        }

        // The expression checked(...), with a lambda in it.
        Console.WriteLine(Safe(() => checked(Run(() => Max + One))));
        Console.WriteLine(Safe(() => unchecked(Run(() => Max + One))));

        // After the block.
        Try("after", () => max + one);

        var add = Make(2);
        Console.WriteLine(add(40));
    }
}
