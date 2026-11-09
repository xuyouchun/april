/* EXPECT_ERROR (memory of the method) */

import System;

using System;
using System.Runtime;

class Program
{
    static Ptr<Int32> Make()
    {
        Ptr<Int32> p = NativeMemory.StackAlloc<Int32>(4);
        Ptr<Int32> q = p + 1;
        return q;
    }

    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine(Make().IsNull);
    }
}
