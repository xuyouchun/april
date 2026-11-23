/* EXPECT_ERROR member "Missing" is not defined in type */
/* EXPECT_NOT variable "Missing" is not defined */
/* EXPECT_NOT no overload of method "WriteLine" */

// A member that does not exist is told once: neither as a variable that is not defined, nor as the
// reason that the call that has it as an argument matches no overload.

import System;

using System;

class Box
{
    public Int32 Value;
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Box box = new Box();
        Console.WriteLine($"{box.Value}, {box.Missing}");
        Console.WriteLine(box.Missing);
    }
}
