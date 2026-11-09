/* EXPECT_ERROR end flag "%>" missing */

import System;
using System.Linq;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Int32[] xs = new Int32[] { 5, 1 };
        var a = <%lq from x in xs select x;
    }
}
