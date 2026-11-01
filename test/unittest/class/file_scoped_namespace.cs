// namespace Name.Sub; : the rest of the file is in the namespace.

/* EXPECT

5
hello

*/

import System;

using System;

namespace Geometry.Shapes;

using System;

class Box
{
    public Int32 Size()
    {
        return 5;
    }
}

namespace Inner
{
class Greeter
{
    public String Hello()
    {
        return "hello";
    }
}
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine(new Box().Size());
        Console.WriteLine(new Inner.Greeter().Hello());
    }
}
