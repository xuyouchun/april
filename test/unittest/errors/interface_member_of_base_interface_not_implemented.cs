/* EXPECT_ERROR does not implement interface member */

import System;

using System;

interface IA
{
    void A();
}

interface IB : IA
{
    void B();
}

abstract class Base : IB
{
    public void B()
    {
    }
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
