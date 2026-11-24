// An auto property can have an initial value: int Number { get; set; } = 5; . It is given when the object is made, before the
// body of the constructor, as the value of a field is.

/* EXPECT

5 hello 0
7 changed
static 10
readonly 3
order 5 15
derived 1 9
default 0 null

*/

import System;

using System;

class Holder
{
    public Int32 Number { get; set; } = 5;
    public String Text { get; set; } = "hello";
    public Int32 Zero { get; set; }
}

class Changed
{
    public Int32 Number { get; set; } = 7;
    public String Text { get; set; } = "init";

    public Changed()
    {
        Text = "changed";
    }
}

class Statics
{
    public static Int32 Count { get; set; } = 10;
}

class ReadOnly
{
    public Int32 Value { get; } = 3;
}

class Order
{
    public Int32 First { get; set; } = 5;
    public Int32 Second { get; set; } = 10 + 5;
}

class Base
{
    public Int32 Number { get; set; } = 1;
}

class Derived : Base
{
    public Int32 Other { get; set; } = 9;
}

class Defaults
{
    public Int32 Number { get; set; } = default;
    public String Text { get; set; } = default;
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Holder h = new Holder();
        Console.WriteLine(h.Number + " " + h.Text + " " + h.Zero);

        Changed c = new Changed();
        Console.WriteLine(c.Number + " " + c.Text);

        Console.WriteLine("static " + Statics.Count);
        Console.WriteLine("readonly " + new ReadOnly().Value);

        Order o = new Order();
        Console.WriteLine("order " + o.First + " " + o.Second);

        Derived d = new Derived();
        Console.WriteLine("derived " + d.Number + " " + d.Other);

        Defaults f = new Defaults();
        Console.WriteLine("default " + f.Number + " " + (f.Text == null? "null" : f.Text));
    }
}
