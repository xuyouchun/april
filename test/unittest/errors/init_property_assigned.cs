/* EXPECT_ERROR has an init accessor: it is set only in an object initializer */

import System;

using System;

class Named
{
    public Int32 Id { get; init; }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Named n = new Named() { Id = 1 };
        n.Id = 2;
    }
}
