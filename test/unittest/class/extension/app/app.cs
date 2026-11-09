// The extension methods of another assembly: found by the marks (ExtensionAttribute) of the
// assembly and of its classes. The class Plain has a method with the same name that is not an
// extension method, it is not called.

import System;
import extlib;

using System;
using extlib;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine("hi".Shout());

        int four = 4;
        Console.WriteLine(four.Twice());
        Console.WriteLine(Plain.Twice(four));
    }
}
