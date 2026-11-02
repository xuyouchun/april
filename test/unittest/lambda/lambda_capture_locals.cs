// A lambda with a block that has local variables, and uses the variables of the method that has it.

/* EXPECT

13

*/

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        int a = 3;
        int b = 4;
        Delegate<Int32> f = () =>
        {
            int r = a * b;
            return r + 1;
        };
        Console.WriteLine(f());
    }
}
