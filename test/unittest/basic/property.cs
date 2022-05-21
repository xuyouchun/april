// Test properties.

/* EXPECT

100

*/

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        // Instance properties get/set values.
        {
            Class1 obj1 = new Class1();
            obj1.Value = 100;

            Console.WriteLine(obj1.Value);
        }
    }
}

class Class1
{
    private int __value;

    public int Value
    {
        get { return __value;  }
        set { __value = value; }
    }
}

