
import System;

using System;
using System.Diagnostics;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
        int v = __GetValue();
        Console.Print(v);
    }

    [Trace]
    private static int __GetValue()
    {
        try
        {
            for(int k = 0; k < 2; k++)
            {
                try
                {
                    Console.WriteLine("__GetValue: Try");
                    return k + 100;
                }
                finally
                {
                    Console.WriteLine("__GetValue: 1");
                }
            }
        }
        finally
        {
            Console.WriteLine("__GetValue: 2");
        }

        return 100;
    }
};


