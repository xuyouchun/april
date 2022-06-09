// Custom struct.

/* EXPECT

2
11
13

*/

import System;

using System;
using Func = System.Delegate<int, int, int>;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        // Simple
        {
            Console.WriteLine((int)Enum1.Item3);
        }

        // Has init values.
        {
            Console.WriteLine((int)Enum2.Item2);
        }

        // Init with other enums types.
        {
            Console.WriteLine((int)Enum3.Item2);
        }

        // Complex reference.
        {
            // TODO:
            // Console.WriteLine((int)Enum4.Item6);
        }
    }
}

enum Enum1
{
    Item1,

    Item2,

    Item3
}

enum Enum2
{
    Item1 = 10,

    Item2,

    Item3
}

enum Enum3
{
    Item1 = Enum2.Item3,

    Item2,

    Item3,

    Item4 = Enum4.Item3,

    Item5
}

enum Enum4
{
    Item1 = Enum3.Item3,

    Item2,

    Item3,

    Item4,

    Item5 = Enum3.Item5,

    Item6,
}

