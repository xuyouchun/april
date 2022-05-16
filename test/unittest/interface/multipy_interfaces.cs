// Multiple interfaces.

/* EXPECT

100
1
2

*/

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        MyClass obj = new MyClass(100);

        Console.WriteLine(obj.GetValue());
        Console.WriteLine(((Interface1)obj).GetValue());
        Console.WriteLine(((Interface2)obj).GetValue());
    }
}

// Interface1
interface Interface1
{
    int GetValue();
}

// Interface2
interface Interface2
{
    int GetValue();
}

// MyClass
class MyClass : Interface1, Interface2
{
    public MyClass(int value)
    {
        __value = value;
    }

    private int __value;

    public int GetValue()
    {
        return __value;
    }

    int Interface1.GetValue()
    {
        return 1;
    }

    int Interface2.GetValue()
    {
        return 2;
    }
}
