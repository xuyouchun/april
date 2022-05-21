// Test simple box.

/* EXPECT

100
101
102
103
104

*/

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        // Constant.
        {
            object obj = 100;
            int value = (int)obj;

            Console.WriteLine(value);
        }

        // Local variable;
        {
            int value = 101;
            object obj = value;

            int r = (int)obj;
            Console.WriteLine(r);
        }

        // Field variable.
        {
            Class1 obj = new Class1();
            obj.Value = 102;

            object valObj = obj.Value;
            int r = (int)valObj;

            Console.WriteLine(r);
        }

        // Argument.
        {
            __TestArgumentBox(103);
        }

        // Return value.
        {
            object valObj = __TestReturnValue();
            int r = (int)valObj;

            Console.WriteLine(r);
        }
    }

    private static void __TestArgumentBox(object valObj)
    {
        int val = (int)valObj;
        Console.WriteLine(val);
    }

    private static object __TestReturnValue()
    {
        return 104;
    }
}

class Class1
{
    public int Value;
}

