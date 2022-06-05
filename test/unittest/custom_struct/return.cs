
// Custom struct - return to variable.

/* EXPECT

4
5
6

*/

import System;

using System;
using Func = System.Delegate<int, int, int>;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        // Return to a variable.
        // -----------------------------------------------------------------------------

        // Return to local variable.
        {
            Struct1 val = __GetStruct1(4);

            Console.WriteLine(val.Value);
        }

        // Return to argument.
        {
            Struct1 val;
            __ReturnToArgument(val);
        }

        // Return to field.
        {
            Class1 obj = new Class1();
            obj.Val = __GetStruct1(6);

            Console.WriteLine(obj.Val.Value);
        }
    }

    private static Struct1 __GetStruct1(int value)
    {
        return new Struct1(value);
    }

    private static void __ReturnToArgument(Struct1 val)
    {
        val = new Struct1(5);
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


class Class2
{
    public Func MyFunc;
}


struct Struct1
{
    public Struct1(int value)
    {
        Value = value;
    }

    public int Value;
}

