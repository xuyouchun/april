// s with { A = 1 } is a copy of the struct s whose members are set; s is not changed.

/* EXPECT

1 2 3
10 2 3
1 20 3
5 6 7
orig 1 2 3
chain 9 8 3
nested 7 2
ok

*/

import System;

using System;

struct Point
{
    public Int32 X;
    public Int32 Y;
    public Int32 Z;
}

class Program
{
    static Point Move(Point p) { return p with { X = p.X + 1 }; }

    [EntryPoint]
    public static void Main()
    {
        Point p = new Point();
        p.X = 1;
        p.Y = 2;
        p.Z = 3;

        Console.WriteLine(p.X + " " + p.Y + " " + p.Z);

        Point a = p with { X = 10 };
        Console.WriteLine(a.X + " " + a.Y + " " + a.Z);

        var b = p with { Y = 20 };
        Console.WriteLine(b.X + " " + b.Y + " " + b.Z);

        Point c = p with { X = 5, Y = 6, Z = 7 };
        Console.WriteLine(c.X + " " + c.Y + " " + c.Z);

        Console.WriteLine("orig " + p.X + " " + p.Y + " " + p.Z);

        Point d = (p with { X = 9 }) with { Y = 8 };
        Console.WriteLine("chain " + d.X + " " + d.Y + " " + d.Z);

        Point m = Move(Move(Move(new Point() with { Y = 2 }))) with { X = 7 };
        Console.WriteLine("nested " + m.X + " " + m.Y);

        Point same = p with { };
        Console.WriteLine(same.X == p.X? "ok" : "no");
    }
}
