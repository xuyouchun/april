// Custom struct.

/* EXPECT

301
302
303

*/

import System;

using System;
using Func = System.Delegate<int, int, int>;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        // Assign param variable to variable.
        // -----------------------------------------------------------------------------

        // Assign param variable to local variable.
        {
            Struct1 val = new Struct1(301);

            __AssignParamToLocal(val);
        }

        // Assign param variable to param variable.
        {
            Struct1 val1, val2;
            val1 = new Struct1(302);

            __AssignParamToParam(val1, val2);
        }

        // Assign param variable to field variable.
        {
            Struct1 val = new Struct1(303);

            __AssignParamToField(val);
        }
    }

    private static void __AssignParamToLocal(Struct1 val)
    {
        Struct1 val1;

        val1 = val;
        Console.WriteLine(val1.Value);
    }

    private static void __AssignParamToParam(Struct1 val1, Struct1 val2)
    {
        val2 = val1;
        Console.WriteLine(val2.Value);
    }

    private static void __AssignParamToField(Struct1 val)
    {
        Class1 obj1 = new Class1();
        obj1.Val = val;

        Console.WriteLine(obj1.Val.Value);
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

