/* EXPECT_ERROR the required member "Name" of ".*Item" must be set by the object initializer */
import System;
using System;

class Item
{
    public required Int32 Id;
    public required String Name { get; set; }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Item a = new Item { Id = 1 };
    }
}
