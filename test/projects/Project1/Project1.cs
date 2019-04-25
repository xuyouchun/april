
import System;

using System;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
        Class1 obj1 = new Class1();
        obj1.Value = 100;

        Console.Print(obj1.Value);
    }
}

class Class1
{
    public int Value
    {
        get; set;
    }
};
