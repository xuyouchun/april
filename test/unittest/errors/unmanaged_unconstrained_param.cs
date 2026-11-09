/* EXPECT_ERROR (unmanaged) */

import System;

using System;

class Program
{
    static T Same<T>(T value) where T : unmanaged => value;

    // T has no constraint: it may be a reference.
    static T Pass<T>(T value) => Same<T>(value);

    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine(Pass(1));
    }
}
