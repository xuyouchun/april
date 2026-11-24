/* EXPECT_ERROR duplicate case "a" */
import System;
using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        switch ("x")
        {
            case "a": Console.WriteLine("1"); break;
            case "a": Console.WriteLine("2"); break;
        }
    }
}
