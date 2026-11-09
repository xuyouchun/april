/* EXPECT_ERROR member names cannot be the same as their enclosing type */

import System;

// An indexer is the member Item: it is the name of this class.
class Item
{
    public Int32 this[Int32 index] { get { return index; } }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Item item = new Item();
        Console.WriteLine(item[1]);
    }
}
