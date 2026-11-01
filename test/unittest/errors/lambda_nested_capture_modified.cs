/* EXPECT_ERROR the local variable "a" is used by a lambda, it cannot be modified */

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        int a = 1;
        Delegate<Delegate<Int32>> f = () => () => a++;
    }
}
