// The arrays that are the arguments of attributes: of numbers and of strings, empty ones, and ones of
// more than one dimension, as the arguments and as the named fields.

/* EXPECT

one 3: 1 1 2
empty-init 0
empty-size 0
empty-strings 0
grid 2x3: 1 2 3 4 5 6
cube 2x2x2: 1 2 3 4 5 6 7 8
words 2x2: a b a c
empty-grid 0x3
named 2: 7 7 rank 1
named-grid 1x2: 9 8
none 0
string-dups 3: x x x

*/

import System;

using System;
using System.Reflection;

class ArraysAttribute : Attribute
{
    public Int32[] Numbers;
    public Int32[,] Grid;

    public ArraysAttribute(Int32[] numbers) { this.Numbers = numbers; }
}

class GridAttribute : Attribute
{
    public Int32[,] Grid;
    public Int32[,,] Cube;
    public String[,] Words;

    public GridAttribute(Int32[,] grid) { this.Grid = grid; }
}

class StringsAttribute : Attribute
{
    public String[] Items;

    public StringsAttribute(String[] items) { this.Items = items; }
}

[Arrays(new Int32[] { 1, 1, 2 })]
class OneOwner { }

[Arrays(new Int32[] { })]
class EmptyInitOwner { }

[Arrays(new Int32[0])]
class EmptySizeOwner { }

[Strings(new String[] { })]
class EmptyStringsOwner { }

[Grid(new Int32[,] { { 1, 2, 3 }, { 4, 5, 6 } })]
class GridOwner { }

[Grid(new Int32[,] { { 0 } }, Cube = new Int32[,,] { { { 1, 2 }, { 3, 4 } }, { { 5, 6 }, { 7, 8 } } })]
class CubeOwner { }

[Grid(new Int32[,] { { 0 } }, Words = new String[,] { { "a", "b" }, { "a", "c" } })]
class WordsOwner { }

[Grid(new Int32[0, 3])]
class EmptyGridOwner { }

[Arrays(new Int32[] { 7, 7 }, Grid = new Int32[,] { { 9, 8 } })]
class NamedOwner { }

[Strings(new String[] { "x", "x", "x" })]
class StringDupsOwner { }

class Program
{
    static String Numbers(Int32[] values)
    {
        String text = values.Length + ":";
        for (Int32 k = 0; k < values.Length; k++)
            text += " " + values[k];

        return values.Length == 0? "0" : text;
    }

    static String Table(Int32[,] grid)
    {
        String text = grid.GetLength(0) + "x" + grid.GetLength(1) + ":";
        for (Int32 i = 0; i < grid.GetLength(0); i++)
        {
            for (Int32 j = 0; j < grid.GetLength(1); j++)
                text += " " + grid[i, j];
        }

        return grid.GetLength(0) * grid.GetLength(1) == 0? grid.GetLength(0) + "x" + grid.GetLength(1) : text;
    }

    [EntryPoint]
    public static void Main()
    {
        Object[] keep = new Object[] { new OneOwner(), new EmptyInitOwner(), new EmptySizeOwner(), new EmptyStringsOwner(),
            new GridOwner(), new CubeOwner(), new WordsOwner(), new EmptyGridOwner(), new NamedOwner(), new StringDupsOwner() };

        Console.WriteLine("one " + Numbers(((ArraysAttribute)typeof(OneOwner).GetAttributes()[0]).Numbers));
        Console.WriteLine("empty-init " + ((ArraysAttribute)typeof(EmptyInitOwner).GetAttributes()[0]).Numbers.Length);
        Console.WriteLine("empty-size " + ((ArraysAttribute)typeof(EmptySizeOwner).GetAttributes()[0]).Numbers.Length);
        Console.WriteLine("empty-strings " + ((StringsAttribute)typeof(EmptyStringsOwner).GetAttributes()[0]).Items.Length);

        Console.WriteLine("grid " + Table(((GridAttribute)typeof(GridOwner).GetAttributes()[0]).Grid));

        Int32[,,] cube = ((GridAttribute)typeof(CubeOwner).GetAttributes()[0]).Cube;
        String text = "cube " + cube.GetLength(0) + "x" + cube.GetLength(1) + "x" + cube.GetLength(2) + ":";
        for (Int32 i = 0; i < 2; i++)
            for (Int32 j = 0; j < 2; j++)
                for (Int32 k = 0; k < 2; k++)
                    text += " " + cube[i, j, k];

        Console.WriteLine(text);

        String[,] words = ((GridAttribute)typeof(WordsOwner).GetAttributes()[0]).Words;
        Console.WriteLine("words " + words.GetLength(0) + "x" + words.GetLength(1) + ": " + words[0, 0] + " "
                          + words[0, 1] + " " + words[1, 0] + " " + words[1, 1]);

        Console.WriteLine("empty-grid " + Table(((GridAttribute)typeof(EmptyGridOwner).GetAttributes()[0]).Grid));

        ArraysAttribute named = (ArraysAttribute)typeof(NamedOwner).GetAttributes()[0];
        Console.WriteLine("named " + named.Numbers.Length + ": " + named.Numbers[0] + " " + named.Numbers[1]
                          + " rank " + named.Numbers.Rank);
        Console.WriteLine("named-grid " + named.Grid.GetLength(0) + "x" + named.Grid.GetLength(1) + ": "
                          + named.Grid[0, 0] + " " + named.Grid[0, 1]);

        Console.WriteLine("none " + (((ArraysAttribute)typeof(EmptyInitOwner).GetAttributes()[0]).Grid == null? 0 : 1));
        String[] dups = ((StringsAttribute)typeof(StringDupsOwner).GetAttributes()[0]).Items;
        Console.WriteLine("string-dups " + dups.Length + ": " + dups[0] + " " + dups[1] + " " + dups[2]);
    }
}
