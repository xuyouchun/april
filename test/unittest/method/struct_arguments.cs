
// Call with struct arguments.

/* EXPECT

300
300

*/

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        // Pass struct as arguments.
        {
            Struct1 struct1 = new Struct1(100, 200);
            int r = __Method1(struct1);

            Console.WriteLine(r);
        }

        // Return struct
        {
            Struct1 struct1 = __GetStruct(100, 200);
            int r = struct1.Value1 + struct1.Value2;

            Console.WriteLine(r);
        }
    }

    private static int __Method1(Struct1 struct1)
    {
        return struct1.Value1 + struct1.Value2;
    }

    private static Struct1 __GetStruct(int value1, int value2)
    {
        return new Struct1(value1, value2);
    }
}


struct Struct1
{
    public Struct1(int value1, int value2)
    {
        Value1 = value1;
        Value2 = value2;
    }

    public int Value1;
    public int Value2;
}
