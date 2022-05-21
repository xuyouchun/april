// Test simple functions.

/* EXPECT

100
3
101
102
103
104
105

*/

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        // Local.
        {
            int val = 100;
            Console.WriteLine(val);
        }

        // Call method.
        {
            int r = __Add(1, 2);
            Console.WriteLine(r);
        }

        // Argument.
        {
            __CallWithArgument(101);
        }

        // Return value.
        {
            int val = __GetValue();
            Console.WriteLine(val);
        }

        // Instance Field.
        {
            Class1 obj1 = new Class1();
            obj1.Field1 = 103;
            Console.WriteLine(obj1.Field1);
        }

        // Instance Property.
        {
            Class1 obj1 = new Class1();
            obj1.Property1 = 104;
            Console.WriteLine(obj1.Property1);
        }

        // Instance auto generated property.
        {
            Class1 obj1 = new Class1();
            obj1.Property2 = 105;
            Console.WriteLine(obj1.Property2);
        }
    }

    private static int __Add(int a, int b)
    {
        return a + b;
    }

    private static void __CallWithArgument(int val)
    {
        Console.WriteLine(val);
    }

    private static int __GetValue()
    {
        return 102;
    }
}

class Class1
{
    public int Field1;

    private int __value1;

    public int Property1
    {
        get { return __value1;  }
        set { __value1 = value; }
    }

    public int Property2 { get; set; }
}






