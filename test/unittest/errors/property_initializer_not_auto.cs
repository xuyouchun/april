/* EXPECT_ERROR has an initial value: only an auto property */
import System;
using System;

class Holder
{
    private Int32 __value;
    public Int32 Number { get { return __value; } set { __value = value; } } = 5;
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine(new Holder().Number);
    }
}
