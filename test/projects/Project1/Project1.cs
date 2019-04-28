
import System;

using System;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
        Class1 obj1 = new Class1(100, 200);

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
    public Class1(int value1, int value2)
    {
        this.Value1 = value1;
        this.Value2 = value2;
    }

    public int Value1 { get; private set; }
    public int Value2 { get; private set; }
};

