// A required member must be set by the object initializer that makes the object: required Int32 Id; required String Name { get; set; } .

/* EXPECT

1 a
2 b 5
3 c
required ok

*/

import System;

using System;

class Item
{
    public required Int32 Id;
    public required String Name { get; set; }
}

class Child : Item
{
    public Int32 Extra = 5;
}

class Holder
{
    public Int32 Number;
    public required Int32 Count { get; init; }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Item a = new Item { Id = 1, Name = "a" };
        Console.WriteLine(a.Id + " " + a.Name);

        Child b = new Child { Id = 2, Name = "b" };
        Console.WriteLine(b.Id + " " + b.Name + " " + b.Extra);

        Item c = new() { Name = "c", Id = 3 };
        Console.WriteLine(c.Id + " " + c.Name);

        Holder h = new Holder { Count = 1, Number = 2 };

        Console.WriteLine("required ok");
    }
}
