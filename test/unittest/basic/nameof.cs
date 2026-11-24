// nameof(x) is the name of what x is, a string that is a constant: the last name of a.b.c is c.

/* EXPECT

value
Length
Name
Console
field
param
Program
const value

*/

import System;

using System;

class Holder
{
    public Int32 Field;
    public String Name { get { return nameof(Name); } }
}

class Program
{
    const String Constant = nameof(Holder);

    static String Param(Int32 param)
    {
        return nameof(param);
    }

    [EntryPoint]
    public static void Main()
    {
        Int32 value = 1;
        Console.WriteLine(nameof(value));
        Console.WriteLine(nameof(String.Empty.Length));
        Console.WriteLine(new Holder().Name);
        Console.WriteLine(nameof(Console));

        Holder holder = new Holder();
        Console.WriteLine(nameof(holder.Field).Length > 0? "field" : "?");
        Console.WriteLine(Param(3) == "param"? "param" : "?");
        Console.WriteLine(nameof(Program));
        Console.WriteLine(Constant == "Holder"? "const value" : "?");
    }
}
