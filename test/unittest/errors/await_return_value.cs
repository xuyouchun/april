/* EXPECT_ERROR "return" cannot be followed by an expression in a void method */

import System;

using System;
using System.Threading.Tasks;

class Program
{
    static Task Work()
    {
        await Task.Delay(1);
        return 5;
    }

    [EntryPoint]
    public static void Main()
    {
        Work().Wait();
    }
}
