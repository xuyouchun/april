/* EXPECT_ERROR (memory of the method) */

import System;

using System;

class Holder
{
    public Ptr<Int32> Saved;
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Int32[] values = new Int32[4];
        Holder holder = new Holder();

        // The pointer is to the memory of the method: a field lives longer.
        holder.Saved = Ptr.AddressOf(values);
        Console.WriteLine(holder.Saved.IsNull);
    }
}
