
// Instance call.

/* EXPECT

-1
100
-1
300

*/

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        // Instance call without arguments & return value.
        {
            Class1 obj1 = new Class1();
            obj1.WithoutReturnAndArguments();

            Console.WriteLine(-1);
        }

        // Instance call with return value.
        {
            Class1 obj1 = new Class1();
            int a = obj1.WithReturn();

            Console.WriteLine(a);
        }

        // Instance call with arguments.
        {
            Class1 obj1 = new Class1();
            obj1.WithArguments(100, 200);

            Console.WriteLine(-1);
        }

        // Instance call with arguments & return value..
        {
            Class1 obj1 = new Class1();
            int a = obj1.WithReturnAndArguments(100, 200);

            Console.WriteLine(a);
        }
    }
}


class Class1
{
    public void WithoutReturnAndArguments()
    {
        // Do nothing.
    }

    public int WithReturn()
    {
        return 100;
    }

    public void WithArguments(int a, int b)
    {
        // Do nothing.
    }

    public int WithReturnAndArguments(int a, int b)
    {
        return a + b;
    }
}

