// Test operators.

/* EXPECT

20
20
123
234

*/

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        // ?:
        {
            int a = 20, b = 10;
            int r = a > b? 20 : 10;

            Console.WriteLine(r);
        }

        // ?: (multipy)
        {
            int a = 20, b = 10, c = 5;
            int r = a > b?
                        a > c? a : b > c? b : c
                    :   b > c? b : c;

            Console.WriteLine(r);
        }

        // ??
        {
            Class1 obj = null;
            Class1 obj2 = obj ?? new Class1(123);

            Console.WriteLine(obj2.Value);
        }

        // ??=
        {
            Class1 obj = null;
            obj ??= new Class1(234);

            Console.WriteLine(obj.Value);
        }
    }
}

class Class1
{
    public Class1(int value)
    {
        Value = value;
    }

    public int Value;
}

