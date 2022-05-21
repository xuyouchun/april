// Test Cast expressions.

/* EXPECT

True
True
False

True
True
False

*/

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        // is
        {
            object obj1 = new Class1(123);
            Console.WriteLine( obj1 is Class1 );
            Console.WriteLine( obj1 is Interface1 );
            Console.WriteLine( obj1 is Interface2 );
        }

        // as
        {
            object obj = new Class1(123);

            Class1 obj1 = obj as Class1;
            Console.WriteLine(obj1 != null);

            Interface1 obj2 = obj as Interface1;
            Console.WriteLine(obj2 != null);

            Interface2 obj3 = obj as Interface2;
            Console.WriteLine(obj3 != null);
        }
    }
}

interface Interface1
{
    int GetValue();
}

interface Interface2
{
    int GetValue();
}

class Class1 : Interface1
{
    public Class1(int value)
    {
        Value = value;
    }

    public int Value;

    public int GetValue()
    {
        return Value;
    }
}
 
