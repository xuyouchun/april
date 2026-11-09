/* EXPECT_ERROR cannot have an initial value, assign it in a method */

import System;

class Box<T>
{
    public static Int32 Count = 10;
    public T Value;
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine(Box<Int32>.Count);
    }
}
