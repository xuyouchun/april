/* EXPECT_ERROR does not implement */

import System;

using System;

interface IAdd<TSelf> where TSelf : IAdd<TSelf>
{
    static abstract TSelf operator +(TSelf a, TSelf b);
}

class P : IAdd<P>
{
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
    }
}
