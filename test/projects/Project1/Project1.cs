
import System;

using System;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
        Class1 obj1 = new Class1();
        obj1[9] = 900;
        int r = obj1[9];

        Console.Print(r);
    }
}

class Class1
{
    public Class1()
    {
        __Array = new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    }

    public int this[int index]
    {
        get
        {
            return __Array[index];
        }
        set
        {
            __Array[index] = value;
        }
    }

    int[] __Array;
};

