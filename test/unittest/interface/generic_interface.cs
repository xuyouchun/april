
// Generic interface.

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
        Interface1<int> obj = new MyClass(100);

        Console.WriteLine(obj.GetValue1());
    }
}

// Interface1
interface Interface1<T>
{
    T GetValue1();
}


// MyClass
class MyClass : Interface1<int>
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
