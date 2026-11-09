// The types of arrays in reflection: their names, the arrays as the params, the results and the
// fields (the type and the rank are checked), the casts, and that an array type has no methods.

/* EXPECT

names Int32[] Int32[, ] String[]
same True True
different False False
no-methods 0
sum 6
make 3
rank 2
get 2
set 4
bad-string InvalidCastException
bad-rank InvalidCastException
bad-element InvalidCastException
bad-arg InvalidCastException
cast 3
is True False False
null-ok True

*/

import System;

using System;
using System.Reflection;

class Tool
{
    public Int32[] Items;
    public Int32[,] Grid;

    public Tool()
    {
        this.Items = new Int32[] { 1, 2 };
        this.Grid = new Int32[2, 2];
    }

    public Int32 Sum(Int32[] values)
    {
        Int32 total = 0;
        for (Int32 k = 0; k < values.Length; k++)
            total += values[k];

        return total;
    }

    public Int32[] Make() { return new Int32[] { 4, 5, 6 }; }

    public Int32[,] Table() { return new Int32[2, 3]; }
}

class Program
{
    static String Kind(Exception e) { return Type.GetType(e).Name; }

    [EntryPoint]
    public static void Main()
    {
        Tool tool = new Tool();
        Type type = Type.GetType(tool);

        Int32[] ints = new Int32[] { 1, 2, 3 };
        Int32[,] grid = new Int32[2, 2];
        String[] words = new String[] { "a" };

        Console.WriteLine("names " + Type.GetType(ints).Name + " " + Type.GetType(grid).Name + " "
                          + Type.GetType(words).Name);

        // The type of an array is the one of the arrays of its items and of its rank.
        Console.WriteLine("same " + Type.GetType(ints).Equals(Type.GetType(new Int32[1])) + " "
                          + Type.GetType(ints).Equals(typeof(Int32[])));
        Console.WriteLine("different " + Type.GetType(ints).Equals(Type.GetType(grid)) + " "
                          + Type.GetType(ints).Equals(Type.GetType(words)));
        Console.WriteLine("no-methods " + Type.GetType(ints).GetMethods().Length);

        Object?[] one = new Object?[] { ints };
        Console.WriteLine("sum " + type.GetMethod("Sum", new Type[] { typeof(Int32[]) }).Invoke(tool, one));

        Object made = type.GetMethod("Make", new Type[0]).Invoke(tool, new Object?[0]);
        Console.WriteLine("make " + ((Int32[])made).Length);

        Object table = type.GetMethod("Table", new Type[0]).Invoke(tool, new Object?[0]);
        Console.WriteLine("rank " + ((Int32[,])table).Rank);

        Field items = type.GetField("Items");
        Console.WriteLine("get " + ((Int32[])items.GetValue(tool)).Length);

        items.SetValue(tool, new Int32[] { 9, 9, 9, 9 });
        Console.WriteLine("set " + tool.Items.Length);

        try { items.SetValue(tool, "bad"); Console.WriteLine("bad-string accepted"); }
        catch (Exception e) { Console.WriteLine("bad-string " + Kind(e)); }

        try { items.SetValue(tool, grid); Console.WriteLine("bad-rank accepted"); }
        catch (Exception e) { Console.WriteLine("bad-rank " + Kind(e)); }

        try { items.SetValue(tool, words); Console.WriteLine("bad-element accepted"); }
        catch (Exception e) { Console.WriteLine("bad-element " + Kind(e)); }

        try
        {
            Object?[] bad = new Object?[] { words };
            type.GetMethod("Sum", new Type[] { typeof(Int32[]) }).Invoke(tool, bad);
            Console.WriteLine("bad-arg accepted");
        }
        catch (Exception e) { Console.WriteLine("bad-arg " + Kind(e)); }

        // The casts.
        Object o = ints;
        Console.WriteLine("cast " + ((Int32[])o).Length);
        Console.WriteLine("is " + (o is Int32[]) + " " + (o is Int32[,]) + " " + (o is String[]));

        // A null is an array of any type.
        items.SetValue(tool, null);
        Console.WriteLine("null-ok " + (tool.Items == null));
    }
}
