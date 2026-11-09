// A type with its namespace is used in expressions: Shapes.Circle.Count, System.Console.

/* EXPECT

2
4
x
3
done

*/

import System;

using System;

namespace Shapes
{
    public class Circle
    {
        public static int Count = 2;

        public static int Twice() { return 4; }
    }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        int n = Shapes.Circle.Count;
        Console.WriteLine(n);
        Console.WriteLine(Shapes.Circle.Twice());

        System.Console.WriteLine("x");
        System.Int32 v = 3;
        System.Console.WriteLine(v);

        Console.WriteLine("done");
    }
}
