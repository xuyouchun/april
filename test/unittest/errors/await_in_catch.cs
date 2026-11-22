/* EXPECT_ERROR it is in a catch or finally block */

import System;

using System;
using System.Threading.Tasks;

class Program
{
    static Task Work()
    {
        try
        {
            Console.WriteLine("try");
        }
        catch (Exception e)
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
