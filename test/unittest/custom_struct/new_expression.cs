
// New expression.

/* EXPECT

1
2
3

*/

import System;

using System;
using Func = System.Delegate<int, int, int>;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        // Assign new expression to a variable.
        // -----------------------------------------------------------------------------

        // Assign new expression to local variable.
        {
            Struct1 val = new Struct1(1);

            Console.WriteLine(val.Value);
        }

        // Assign new expression to argument.
        {
            Struct1 val;
            __AssignNewToArgument(val);
        }

        // Assign new expression to field.
        {
            Class1 obj = new Class1();
            obj.Val = new Struct1(3);

            Console.WriteLine(obj.Val.Value);
        }
    }

    private static void __AssignNewToArgument(Struct1 val)
    {
        val = new Struct1(2);
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

