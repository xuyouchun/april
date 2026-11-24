/* EXPECT_ERROR cannot create an instance of the generic param .*new\(\) or struct constraint */
/* EXPECT_AT 8:30 */
import System;
using System;

class Maker<X>
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
