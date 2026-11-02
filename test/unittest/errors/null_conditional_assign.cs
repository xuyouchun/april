/* EXPECT_ERROR null-conditional */

import System;

using System;

class Node
{
    public int N;
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Node n = new Node();
        n?.N = 1;
    }
}
