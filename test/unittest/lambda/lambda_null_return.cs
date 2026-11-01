// The type of null is Object: a lambda that returns null has that type, if it is not converted to a delegate.

/* EXPECT

True

True

negative
positive
True

True

True

*/

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        var a = () => null;
        Object r = a();
        Console.WriteLine(r == null);

        var b = (Int32 x) =>
        {
            if (x > 0)
            {
                return null;
            }

            return "negative";
        };
        Console.WriteLine(b(1) == null);
        Console.WriteLine((String)b(-1));

        var c = (Int32 x) =>
        {
            if (x > 0)
            {
                return "positive";
            }

            return null;
        };
        Console.WriteLine(c(1));
        Console.WriteLine(c(0) == null);

        Console.WriteLine((() => { return null; })() == null);

        // Converted to a delegate: the type is that of the delegate.
        Delegate<String, Int32> s = (Int32 x) => { return null; };
        Console.WriteLine(s(1) == null);
    }
}
