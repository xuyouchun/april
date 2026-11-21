// The static members of a base class are found by their simple names in the derived class: a method, a field, a constant, and a
// static property (an instance property of the base too).

/* EXPECT

8 4 12 6 3

*/

import System;

using System;

abstract class Base
{
    public static Int32 Field = 3;
    public const Int32 Constant = 11;

    public static Int32 Property => 5;

    public Int32 Instance => 2;

    public static Int32 Method() { return 7; }
}

class Derived : Base
{
    public Int32 FromMethod() { return Method() + 1; }

    public Int32 FromField() { return Field + 1; }

    public Int32 FromConstant() { return Constant + 1; }

    public Int32 FromProperty() { return Property + 1; }

    public Int32 FromInstance() { return Instance + 1; }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Derived d = new Derived();
        Console.WriteLine("" + d.FromMethod() + " " + d.FromField() + " " + d.FromConstant() + " " + d.FromProperty() + " " + d.FromInstance());
    }
}
