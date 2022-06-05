// Custom struct - local.

/* EXPECT

100
101
102

*/

import System;

using System;
using Func = System.Delegate<int, int, int>;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        // Assign local variable to variable.
        // -----------------------------------------------------------------------------

        // Assign local variable to local variable.
        {
            Struct1 val = new Struct1(100);
            Struct1 val2;

            val2 = val;
            Console.WriteLine(val2.Value);
        }

        // Assign local variable to param variable.
        {
            Struct1 val;
            __AssignLocalToParam(val);
        }

        // Assign local variable to field variable.
        {
            Class1 obj = new Class1();
            obj.Val = new Struct1(102);

            Console.WriteLine(obj.Val.Value);
        }
    }

    private static void __AssignLocalToParam(Struct1 val)
    {
        Struct1 val2 = new Struct1(101);
        val = val2;

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

