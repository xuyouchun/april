// Test custom struct box.

/* EXPECT

123
234
999
345

*/

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        // Local.
        {
            Struct1 val1 = new Struct1(123);
            object obj1 = val1;
            Struct1 val2 = (Struct1)obj1;

            Console.WriteLine(val2.Value);
        }

        // Arguments.
        {
            __PassAsArgument(234);
        }

        // Field
        {
            Class1 obj = new Class1();
            obj.Val1 = new Struct1(999);

            object obj1 = obj.Val1;
            Struct1 val2 = (Struct1)obj1;
            Console.WriteLine(val2.Value);
        }

        // ReturnValue
        {
            object obj1 = __ReturnValue();
            Struct1 val1 = (Struct1)obj1;

            Console.WriteLine(val1.Value);
        }
    }

    private static void __PassAsArgument(object obj)
    {
        Struct1 val1 = (Struct1)obj;
        Console.WriteLine(val1.Value);
    }

    private static object __ReturnValue()
    {
        return new Struct1(345);
    }
}


struct Struct1
{
    public Struct1(int value)
    {
        Value = value;
    }

    public int Value;
}


class Class1
{
    public Struct1 Val1;
}
