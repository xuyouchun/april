// An enum that has [Flags]: the text of a value that is the or of items is the names of the items, separated by commas;
// a value that has an item keeps its name, a bit that no item has gives the number. An enum without [Flags] gives the number.

/* EXPECT

Read, Write
Read
All
None
Read, Execute
9
3
Read, Write

*/

import System;

using System;

[Flags]
enum Perm
{
    None = 0, Read = 1, Write = 2, Execute = 4, All = 7;
}

[Flags]
enum Small : UInt8
{
    A = 1, B = 2;
}

enum Plain
{
    X = 1, Y = 2;
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine(Perm.Read | Perm.Write);
        Console.WriteLine(Perm.Read);
        Console.WriteLine(Perm.All);
        Console.WriteLine(Perm.None);

        Console.WriteLine((Perm.Read | Perm.Execute).ToString());
        Console.WriteLine((Small)9);
        Console.WriteLine((Plain)3);

        Perm p = Perm.Read;
        p = p | Perm.Write;
        Console.WriteLine(p.ToString());
    }
}
