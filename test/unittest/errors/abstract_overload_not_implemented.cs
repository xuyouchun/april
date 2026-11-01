/* EXPECT_ERROR does not implement inherited abstract member */

import System;

using System;

abstract class Base
{
    public abstract void F(int x);
}

class Derived : Base
{
    // Another signature: it does not implement F(int).
    public override void F(String s)
    {
    }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
    }
}
