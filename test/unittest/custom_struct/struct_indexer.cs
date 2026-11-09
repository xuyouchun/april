// The indexer of a struct: read from a local, a parameter, a field and the value of a call; an indexer that returns
// a struct; and the temps of a statement that do not take the place of the variables.

/* EXPECT

local 20 7 25
param 30 4
field 50 3
call 20 40 50
set 99 0 1
struct 3 4 | 5 6 | 3 4
twice 1 1 5
0 3 0
statements 5 3 4 2

*/

import System;

using System;

struct Window
{
    public Window(Int32[] items, Int32 start, Int32 count)
    {
        __items = items;
        __start = start;
        __count = count;
    }

    public Int32 Count => __count;

    public Int32 this[Int32 index]
    {
        get { return __items[__start + index]; }
        set { __items[__start + index] = value; }
    }

    public Window Cut(Int32 start) => new Window(__items, __start + start, __count - start);

    private Int32[] __items;

    private Int32 __start;

    private Int32 __count;
}

struct Point
{
    public Point(Int32 x, Int32 y)
    {
        X = x;
        Y = y;
    }

    public Int32 X;

    public Int32 Y;
}

struct Grid
{
    public Grid(Point[] points)
    {
        __points = points;
    }

    public Point this[Int32 index] => __points[index];

    private Point[] __points;
}

class Holder
{
    public Window Content;
}

class Program
{
    static Int32 Second(Window w) => w[1];

    [EntryPoint]
    public static void Main()
    {
        Int32[] items = new Int32[] { 10, 20, 30, 40, 50 };
        Window all = new Window(items, 0, 5);

        // A local, in a text and in a sum.
        Int32 x = all[1];
        Console.WriteLine("local " + all[1] + " " + (x - 13) + " " + (all[0] + 15));

        // A parameter, and the window that is made from it.
        Console.WriteLine("param " + Second(all.Cut(1)) + " " + all.Cut(1).Count);

        // A field of an object.
        Holder holder = new Holder();
        holder.Content = new Window(items, 2, 3);
        Console.WriteLine("field " + holder.Content[2] + " " + holder.Content.Count);

        // The value of a call is the object that is indexed, to read and to write.
        Console.WriteLine("call " + all.Cut(1)[0] + " " + all.Cut(1).Cut(1)[1] + " " + all.Cut(3)[1]);

        all.Cut(2)[0] = 99;
        Window head = new Window(items, 0, 2);
        head[1] = 0;
        Console.WriteLine("set " + items[2] + " " + items[1] + " " + (head[0] == 10 ? 1 : 0));

        // An indexer that returns a struct: its member is read.
        Point[] points = new Point[3];
        points[0] = new Point(3, 4);
        points[1] = new Point(5, 6);
        points[2] = new Point(7, 8);
        Grid grid = new Grid(points);
        Point first = grid[0];
        Console.WriteLine("struct " + grid[0].X + " " + grid[0].Y + " | " + grid[1].X + " " + grid[1].Y + " | " + first.X + " " + first.Y);

        // Two temps of a struct in one statement, and a temp in a later one: the variables keep their values.
        Window w = new Window(items, 0, 5);
        Console.WriteLine("twice " + new Window(items, 0, 1).Count + " " + new Window(items, 0, 1).Count + " " + w.Count);
        Int32 q = 1;
        Console.WriteLine("" + w.Cut(5).Count + " " + w.Cut(2).Count + " " + (q - 1));
        Window mid = w.Cut(2);
        Window two = w.Cut(1);
        Window inner = mid.Cut(1);
        Console.WriteLine("statements " + w.Count + " " + mid.Count + " " + two.Count + " " + inner.Count);
    }
}
