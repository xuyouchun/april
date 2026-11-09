/* EXPECT_ERROR takes one parameter */

import System;

using System;

struct M
{
    public Int32 V;

    public static implicit operator Int32(M a, M b) { return a.V + b.V; }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
    }
}
