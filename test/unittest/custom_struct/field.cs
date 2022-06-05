// Custom struct.

/* EXPECT

201
202
203

*/

import System;

using System;
using Func = System.Delegate<int, int, int>;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        // Assign field variable to local variable.
        // -----------------------------------------------------------------------------

        // Assign field variable to local variable.
        {
            Class1 obj1 = new Class1(201);
            Struct1 val1;

            val1 = obj1.Val;
            Console.WriteLine(val1.Value);
        }

        // Assign field variable to param variable.
        {
            Struct1 val;
            __AssignFieldToParam(val);
        }

        // Assign field variable to field variable.
        {
            Class1 obj1 = new Class1(203);
            Class1 obj2 = new Class1();

            obj2.Val = obj1.Val;
            Console.WriteLine(obj2.Val.Value);
        }
    }

    private static void __AssignFieldToParam(Struct1 val)
    {
        Class1 obj1 = new Class1(202);
        val = obj1.Val;

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

    public void Call()
    {
        Val = new Struct1(5);

        Console.WriteLine(Val.Value);
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

