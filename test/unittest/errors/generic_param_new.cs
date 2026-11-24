/* EXPECT_ERROR new \?X\(\)\) is not supported */
import System;
using System;

class Maker<X> where X : new()
{
    public X Make() { return new X(); }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine(new Maker<Object>().Make() != null);
    }
}
