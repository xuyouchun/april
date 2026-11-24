/* EXPECT_ERROR invalid expression "k" */
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
            case 1 when k: Console.WriteLine("x"); break;
        }
    }
}
