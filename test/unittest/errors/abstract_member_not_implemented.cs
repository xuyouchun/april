/* EXPECT_ERROR does not implement inherited abstract member */

import System;

using System;

abstract class Base
{
    public abstract void F(int x);
}

class Derived : Base
{
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
    }
}
