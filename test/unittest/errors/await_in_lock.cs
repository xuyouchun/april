/* EXPECT_ERROR it is in a lock statement */

import System;

using System;
using System.Threading.Tasks;

class Program
{
    static Object __gate = new Object();

    static Task Work()
    {
        lock (__gate)
        {
            await Task.Delay(1);
        }
    }

    [EntryPoint]
    public static void Main()
    {
        Work().Wait();
    }
}
