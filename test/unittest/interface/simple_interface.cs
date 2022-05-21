
// Simple interface.

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
        Interface1 obj = new MyClass(100);
        int r = obj.GetValue1();

        Console.WriteLine(r);
    }
}

// Interface1
interface Interface1
{
    int GetValue1();
}


// MyClass
class MyClass : Interface1
{
    public MyClass(int value)
    {
        __value = value;
    }

    private int __value;

    public int GetValue1()
    {
        return __value;
    }
}
