// Custom struct.

/* EXPECT

401
402
403

*/

import System;

using System;
using Func = System.Delegate<int, int, int>;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        // Return a custom struct.
        // -----------------------------------------------------------------------------

        // Return a local variable.
        {
            Struct1 val = __ReturnLocal();
            Console.WriteLine(val.Value);
        }

        // Return a argument variable.
        {
            Struct1 val = new Struct1(402);
            Struct1 val1 = __ReturnParam(val);

            Console.WriteLine(val1.Value);
        }

        // Return a field variable.
        {
            Struct1 val = __ReturnField();
            Console.WriteLine(val.Value);
        }
    }

    private static Struct1 __ReturnLocal()
    {
        return new Struct1(401);
    }

    private static Struct1 __ReturnParam(Struct1 val)
    {
        return val;
    }

    private static Struct1 __ReturnField()
    {
        Class1 obj1 = new Class1(403);
        return obj1.Val;
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

