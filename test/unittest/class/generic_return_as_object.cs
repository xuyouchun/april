// The result of a generic method that returns its type param, used as an Object or in a string concatenation.

/* EXPECT

z
a z
a 5
7 x
same class z y 4 z
done

*/

import System;

using System;

class Util
{
    public static T Same<T>(T value)
    {
        return value;
    }
}

class Program
{
    static T Same<T>(T value)
    {
        return value;
    }

    static String Make(String a)
    {
        return a + "y";
    }

    [EntryPoint]
    public static void Main()
    {
        Object o = Util.Same("z");
        Console.WriteLine("" + o);

        String s = "a " + Util.Same("z");
        Console.WriteLine(s);

        Console.WriteLine("a " + Util.Same(5));

        Object p = Util.Same(7);
        Console.WriteLine(p + " " + Util.Same("x"));
        // The method is of the same class (called by its name).
        Object q = Same("z");
        Object r = Make("");
        Object n = Same(4);
        Console.WriteLine("same class " + q + " " + r + " " + n + " " + Same("z"));
        Console.WriteLine("done");
    }
}
