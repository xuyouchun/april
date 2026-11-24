/* EXPECT_ERROR the cases of the switch are strings, its value is of the type "System.Int32" */
import System;
using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Int32 k = 1;
        switch (k)
        {
            case "a": Console.WriteLine("1"); break;
        }
    }
}
