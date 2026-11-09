// The arrays of many dimensions are derived from Array<T>, like the arrays of one dimension. For
// Array<T> an array has one dimension: the items are in the order of the rows (the last index
// changes first). An array is read by one index for each dimension, or by one index (the position
// in that order). foreach goes through the items in that order, whatever the dimensions are.
//
// The index of each dimension is not checked: only the position is (a 2x3 array read at [0, 3]
// gives the item [1, 0]), it is made so for the speed of the programs.

/* EXPECT

create 7 2 6 2 3
initializer 4 2 3 6
three 9 3 8 2
flat 5 5 60 60
foreach 123456
foreach3 12345678
foreach var 1 2 3 4
base 6 4 5 True
interface 6 21 6
linq 3 3 2
rows 2 2 5
get length True
position 4
done

*/

import System;

using System;
using System.Collections;
using System.Linq;

class Program
{
    static Int32 Total(Array<Int32> items)
    {
        Int32 total = 0;
        for (Int32 i = 0; i < items.Length; i++)
            total += items[i];

        return total;
    }

    [EntryPoint]
    public static void Main()
    {
        // Two dimensions.
        Int32[,] m = new Int32[2, 3];
        m[1, 2] = 7;
        Console.WriteLine("create " + m[1, 2] + " " + m.Rank + " " + m.Length + " " + m.GetLength(0)
                                    + " " + m.GetLength(1));

        Int32[,] init = new Int32[,] { { 1, 2, 3 }, { 4, 5, 6 } };
        Console.WriteLine("initializer " + init[1, 0] + " " + init.GetLength(0) + " "
                                        + init.GetLength(1) + " " + init[1, 2]);

        // Three dimensions.
        Int32[,,] c = new Int32[2, 2, 2];
        c[1, 1, 1] = 9;
        Console.WriteLine("three " + c[1, 1, 1] + " " + c.Rank + " " + c.Length + " " + c.GetLength(2));

        // The array of any dimension is read and written by one index.
        Int32 before = init[4];
        init[5] = 60;
        Console.WriteLine("flat " + before + " " + init[1, 1 + 0] + " " + init[5]
                                 + " " + init[1, 2]);
        init[5] = 6;

        // foreach goes through the items in the order of the rows.
        String items = "";
        foreach (Int32 v in init)
            items = items + v;

        Console.WriteLine("foreach " + items);

        Int32[,,] cube = new Int32[,,] { { { 1, 2 }, { 3, 4 } }, { { 5, 6 }, { 7, 8 } } };
        String cubeItems = "";
        foreach (Int32 v in cube)
            cubeItems = cubeItems + v;

        Console.WriteLine("foreach3 " + cubeItems);

        String vars = "";
        foreach (var v in new Int32[,] { { 1, 2 }, { 3, 4 } })
            vars = vars + (vars == ""? "" : " ") + v;

        Console.WriteLine("foreach var " + vars);

        // The base type is Array<T>: it is read as an array of one dimension.
        Array<Int32> flat = init;
        Console.WriteLine("base " + flat.Length + " " + flat[3] + " " + Total(new Int32[] { 1, 4 })
                               + " " + (flat is Array<Int32>));

        // The collections.
        IEnumerable<Int32> enumerable = init;
        Int32 sum = 0;
        foreach (Int32 v in enumerable)
            sum += v;

        IReadOnlyList<Int32> list = init;
        Console.WriteLine("interface " + list.Count + " " + sum + " " + list[5]);

        Console.WriteLine("linq " + init.Where(v => v % 2 == 0).Count() + " " + init.Skip(2).First()
                                 + " " + init.Take(2).Last());

        // The arrays of arrays.
        Int32[][] rows = new Int32[][] { new Int32[] { 1, 2 }, new Int32[] { 3 } };
        Console.WriteLine("rows " + rows.Length + " " + rows[0].Length + " " + (rows[0][1] + rows[1][0]));

        // The dimension that is asked for must be one of the array.
        Boolean outOfRange = false;
        try
        {
            Int32 length = m.GetLength(2);
        }
        catch (OutOfRangeException)
        {
            outOfRange = true;
        }

        Console.WriteLine("get length " + outOfRange);

        // A dimension is not checked by itself, the position is.
        Console.WriteLine("position " + init[0, 3]);

        Console.WriteLine("done");
    }
}
