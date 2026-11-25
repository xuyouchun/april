/* EXPECT_ERROR copies a struct */
import System;
using System;

class Item
{
    public Int32 Id;
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Item a = new Item();
        Item b = a with { Id = 1 };
    }
}
