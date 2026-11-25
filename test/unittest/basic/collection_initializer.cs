// new List<T> { a, b } calls Add for each value; { k, v } has two arguments: Add(k, v). The values can be mixed with the members
// that are set (Name = value).

/* EXPECT

list 3 1 2 3
dict 2 10 20
mixed 2 label
nested 2 2
empty 0
trailing 2

*/

import System;

using System;
using System.Collections;

class Bag
{
    public String Label;
    public List<Int32> Items = new();

    public void Add(Int32 item) { Items.Add(item); }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        List<Int32> list = new List<Int32> { 1, 2, 3 };
        Console.WriteLine("list " + list.Count + " " + list[0] + " " + list[1] + " " + list[2]);

        Dictionary<Int32, Int32> dict = new Dictionary<Int32, Int32> { { 1, 10 }, { 2, 20 } };
        Console.WriteLine("dict " + dict.Count + " " + dict[1] + " " + dict[2]);

        Bag bag = new Bag { Label = "label", 1, 2 };
        Console.WriteLine("mixed " + bag.Items.Count + " " + bag.Label);

        List<List<Int32>> nested = new List<List<Int32>> { new List<Int32> { 1, 2 }, new List<Int32> { 3 } };
        Console.WriteLine("nested " + nested.Count + " " + nested[0].Count);

        List<Int32> empty = new List<Int32> { };
        Console.WriteLine("empty " + empty.Count);

        List<Int32> trailing = new List<Int32> { 1, 2, };
        Console.WriteLine("trailing " + trailing.Count);
    }
}
