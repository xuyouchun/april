
// Differents this arguments.

/* EXPECT

111
222

*/

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        // Class object
        {
            Class1 obj1 = new Class1(111);
            int r = obj1.GetValue();

            Console.WriteLine(r);
        }

        // Custom struct
        {
            Struct1 st1 = new Struct1(222);
            int r = st1.GetValue();

            Console.WriteLine(r);
        }

        // System value types
        {
            int value = 100;
        }
    }
}

class Class1
{
    public Class1(int value)
    {
        __value = value;
    }

    private int __value;

    public int GetValue()
    {
        return __value;
    }
}

class Struct1
{
    public Struct1(int value)
    {
        __value = value;
    }

    private int __value;

    public int GetValue()
    {
        return __value;
    }
}


