// Custom struct.

/* EXPECT

501
502
503

*/

import System;

using System;
using Func = System.Delegate<int, int, int>;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        // Pass an argument.
        // -----------------------------------------------------------------------------

        // Pass a local variable as argument.
        {
            Struct1 val = new Struct1(501);
            __ParseLocalAsArgument(val);
        }

        // Pass an argument variable as argument.
        {
            Struct1 val = new Struct1(502);
            __ParseArgumentAsArgument(val);
        }

        // Pass a file variable as argument.
        {
            Class1 obj1 = new Class1(503);
            __ParseFieldAsArgument(obj1.Val);
        }
    }

    private static void __ParseLocalAsArgument(Struct1 val)
    {
        Console.WriteLine(val.Value);
    }

    private static void __ParseArgumentAsArgument(Struct1 val)
    {
        __ParseArgumentAsArgument_2(val);
    }

    private static void __ParseArgumentAsArgument_2(Struct1 val)
    {
        Console.WriteLine(val.Value);
    }

    private static void __ParseFieldAsArgument(Struct1 val)
    {
        Console.WriteLine(val.Value);
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

