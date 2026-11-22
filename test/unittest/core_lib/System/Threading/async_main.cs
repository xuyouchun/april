// The entry point can return Task: the program waits for it (the method is a state machine).

/* EXPECT

before
after the delay

*/

import System;

using System;
using System.Threading.Tasks;

class Program
{
    [EntryPoint]
    public static Task Main()
    {
        Console.WriteLine("before");
        await Task.Delay(30);
        Console.WriteLine("after the delay");
    }
}
