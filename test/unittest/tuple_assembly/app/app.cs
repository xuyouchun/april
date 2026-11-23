// The names of the items of a tuple are in the metadata of the library.

import System;
import lib;

using System;
using System.Collections;
using lib.Geo;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Shape shape = new Shape();
        Console.WriteLine(shape.Size.Width + "x" + shape.Size.Height);
        Console.WriteLine(shape.Info.Name + " " + shape.Info.Sides);
        Console.WriteLine(Shape.Make(7).Label + Shape.Make(7).Id);
        Console.WriteLine(Shape.Area((5, 6)));

        Point p = (8, 9);
        Console.WriteLine(p.X + "," + p.Y);

        Pair<String> q = ("a", "b");
        Console.WriteLine(q.Second + q.First);

        // The names in the generic arguments.
        Console.WriteLine(shape.Rows[1].Label + shape.Rows[1].Id);

        foreach (var row in shape.Rows)
            Console.WriteLine(row.Label);

        Console.WriteLine(Shape.Ranges()["a"].Hi);
        Console.WriteLine(Shape.Codes()[0].Text + Shape.Codes()[0].Code);

        Table table = new Table();
        table.Add((2, 7));
        Console.WriteLine(table[0].Column);
    }
}
