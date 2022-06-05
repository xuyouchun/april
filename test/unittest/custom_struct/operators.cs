// Custom struct.

/* EXPECT

601
602
603

*/

import System;

using System;
using Func = System.Delegate<int, int, int>;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        // ?: operator.
        // -----------------------------------------------------------------------------

        // Simple.
        {
            Struct1 val1 = new Struct1(601);
            Struct1 val2 = new Struct1(602);

            Struct1 val = true? val1 : val2;
            Console.WriteLine(val.Value);
        }

        // Complex.
        {
            Struct1 val1 = new Struct1(604);

            Struct1 val = true? __GetStruct1(602) :
                          true? new Struct1(603) : val1;

            Console.WriteLine(val.Value);
        }

        // Delegate
        {
            Delegate<int, int, int> func = true? __Add : __Sub;
            int v = func(600, 3);
            Console.WriteLine(v);
        }
    }

    private static Struct1 __GetStruct1(int value)
    {
        return new Struct1(value);
    }

    private static int __Add(int a, int b)
    {
        return a + b;
    }

    private static int __Sub(int a, int b)
    {
        return a - b;
    }
}

class Class1
{
    public Class1()
    {

    }

    public Class1(int value)
    {
        Val = new Struct1(value);
    }

    public Struct1 Val;
}

struct Struct1
{
    public Struct1(int value)
    {
        Value = value;
    }

    public int Value;
}

