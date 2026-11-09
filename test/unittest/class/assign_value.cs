// The value of an assignment is the value that is assigned, whatever the destination is: a
// variable, a field, a property, an indexer, an element of an array of any dimension.

/* EXPECT

local 3 3
field 4 4
static 5 5
property 6 6
indexer 8 8
array 5 5
array 7 7
matrix 9 9
compound 8 8
chain 2 2 2
write 11
statement 3 2

*/

import System;

using System;

class Cell
{
    public Int32 F;
    public static Int32 S;
    public Int32 P { get; set; }

    public Int32 this[Int32 index]
    {
        get => this.F;
        set => this.F = value;
    }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Int32 l = 0;
        Int32 a1 = (l = 3);
        Console.WriteLine("local " + a1 + " " + l);

        Cell cell = new Cell();
        Int32 a2 = (cell.F = 4);
        Console.WriteLine("field " + a2 + " " + cell.F);

        Int32 a3 = (Cell.S = 5);
        Console.WriteLine("static " + a3 + " " + Cell.S);

        Int32 a4 = (cell.P = 6);
        Console.WriteLine("property " + a4 + " " + cell.P);

        Int32 a5 = (cell[1] = 8);
        Console.WriteLine("indexer " + a5 + " " + cell.F);

        Int32[] a = new Int32[3];
        Int32 x = (a[1] = 5);
        Console.WriteLine("array " + x + " " + a[1]);

        Int32 y = a[2] = 7;
        Console.WriteLine("array " + y + " " + a[2]);

        Int32[,] m = new Int32[2, 2];
        Int32 z = (m[1, 1] = 9);
        Console.WriteLine("matrix " + z + " " + m[1, 1]);

        a[0] = 3;
        Int32 q = (a[0] += 5);
        Console.WriteLine("compound " + q + " " + a[0]);

        Int32 c = l = cell.F = 2;
        Console.WriteLine("chain " + c + " " + l + " " + cell.F);

        Console.WriteLine("write " + (a[2] = 11));

        // As a statement the value is not left on the stack.
        Int32 i = 0;
        a[i++] = 3;
        cell.P = 2;
        Console.WriteLine("statement " + a[0] + " " + cell.P);
    }
}
