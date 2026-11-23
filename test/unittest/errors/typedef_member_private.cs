/* EXPECT_ERROR is inaccessible */

import System;

using System;
using System.Collections;

class Holder
{
    private typedef Items = List<Int32>;
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Holder.Items i = null;
    }
}
