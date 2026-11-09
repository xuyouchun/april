// The initial value of a field can be any expression: it is evaluated by the constructor (of the objects,
// or of the type for a static field), after the constants.

/* EXPECT

items 3 20
grid 2 3
box 5
static 3 9
call 12
order 1 2 3
chain 7
again 3

*/

import System;

using System;

class Box
{
    public Int32 Value;

    public Box() { this.Value = 5; }
}

class Holder
{
    public Int32[] Items = new Int32[] { 10, 20, 30 };
    public Int32[,] Grid = new Int32[2, 3];
    public Box Box = new Box();
    public Int32 Called = Holder.Make(4);
    public Int32 Plain = 1;

    public static Int32[] Table = new Int32[] { 1, 4, 9 };
    public static Int32 Made = Holder.Make(3) + 0;

    static Int32 Make(Int32 n) { return n * 3; }

    public Int32 Order1 = 1;
    public Int32 Order2 = 2;
    public Int32 Order3 = 3;

    // The values of the constructor come after.
    public Int32 Late = 0;

    public Holder() { this.Late = 7; }

    // Another constructor: the fields are made too.
    public Holder(Int32 late) { this.Late = late; }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Holder h = new Holder();
        Console.WriteLine("items " + h.Items.Length + " " + h.Items[1]);
        Console.WriteLine("grid " + h.Grid.GetLength(0) + " " + h.Grid.GetLength(1));
        Console.WriteLine("box " + h.Box.Value);
        Console.WriteLine("static " + Holder.Table.Length + " " + Holder.Table[2]);
        Console.WriteLine("call " + h.Called);
        Console.WriteLine("order " + h.Order1 + " " + h.Order2 + " " + h.Order3);
        Console.WriteLine("chain " + h.Late);

        Holder other = new Holder(3);
        Console.WriteLine("again " + other.Items.Length);
    }
}
