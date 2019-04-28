
import System;

using System;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
        Class1 obj1 = new Class1();

        obj1.Value1 = 100;
        obj1.Value2 = 200;

        Console.Print(obj1.Value1 + obj1.Value2);
    }

    public static int Test1()
    {
        int a = 0, b = 10;

        return a + b;
    }
};

class Class1
{
    public int Value1 { get; set; }
    public int Value2 { get; set; }
};

