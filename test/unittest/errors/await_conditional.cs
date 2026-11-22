/* EXPECT_ERROR it is in an expression that does not always run */

import System;

using System;
using System.Threading.Tasks;

class Program
{
    static Task<Boolean> Work(Boolean first)
    {
        return first && await Task.FromResult(true);
    }

    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine(Work(true).Result);
    }
}
