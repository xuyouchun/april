// User conversions: explicit operator, explicit operator checked (used in a checked context), implicit operator (used
// by the initial value of a variable, an assignment, a return, and the arguments of a call); an explicit cast takes the
// implicit operator too.

/* EXPECT

explicit 7 7000 6 9 K9
explicit again 8 K2
checked overflow
implicit 4 4 9 6
argument 11 20
cast of implicit 5 12
many 4 40 M4
constructor 7 9
operators 6 5 True True -5 15
done

*/

import System;

using System;

struct Meters
{
    public Int32 V;

    public static Meters Make(Int32 v)
    {
        Meters m = new Meters();
        m.V = v;
        return m;
    }

    // The conversion in a checked context makes millimeters, and raises when they do not fit.
    public static explicit operator Int32(Meters m) { return m.V; }

    public static explicit operator checked Int32(Meters m) { return checked(m.V * 1000); }

    public static explicit operator Meters(Int32 v) { return Make(v + 1); }
}

class Kilo
{
    public Int32 V;

    public static Kilo Make(Int32 v)
    {
        Kilo k = new Kilo();
        k.V = v;
        return k;
    }

    public static explicit operator Kilo(Int32 v) { return Make(v); }

    public static explicit operator String(Kilo k) { return "K" + k.V; }
}

struct Tagged
{
    public Int32 V;

    public static implicit operator Tagged(Int32 v)
    {
        Tagged t = new Tagged();
        t.V = v;
        return t;
    }

    public static implicit operator Int32(Tagged t) { return t.V; }
}

// The conversions of a type to other types: they have the same parameter, they differ in what they make.
struct Many
{
    public Int32 V;

    public static Many Make(Int32 v)
    {
        Many m = new Many();
        m.V = v;
        return m;
    }

    public static explicit operator Int32(Many m) { return m.V; }

    public static explicit operator Int64(Many m) { return (Int64)m.V * 10; }

    public static explicit operator String(Many m) { return "M" + m.V; }
}

class Holder
{
    public Tagged T;

    public Holder(Tagged t) { T = t; }
}

class Derived : Holder
{
    public Derived(Int32 v) : base(v) { }
}

class Program
{
    static Tagged Pass(Int32 v) { return v; }

    static Int32 Plus(Tagged t, Tagged u) { return t.V + u.V + 1; }

    [EntryPoint]
    public static void Main()
    {
        Meters m = Meters.Make(7);
        Int32 plain = (Int32)m;
        Int32 big = checked((Int32)m);
        Meters back = (Meters)5;
        Kilo k = (Kilo)9;
        String s = (String)k;
        Console.WriteLine("explicit " + plain + " " + big + " " + back.V + " " + k.V + " " + s);
        Console.WriteLine("explicit again " + ((Int32)m + 1) + " " + (String)Kilo.Make(2));

        String overflow;
        try
        {
            Int32 x = checked((Int32)Meters.Make(3000000));
            overflow = "no " + x;
        }
        catch (OverflowException)
        {
            overflow = "overflow";
        }

        Console.WriteLine("checked " + overflow);

        Tagged t = 4;
        Int32 n = t;
        t = 9;
        Tagged r = Pass(6);
        Console.WriteLine("implicit " + n + " " + n + " " + t.V + " " + r.V);

        Int32 a = Plus(4, 6);
        Int32 b = Plus(t, 10);
        Console.WriteLine("argument " + a + " " + b);

        Tagged c = (Tagged)5;
        Int32 d = (Int32)Pass(12);
        Console.WriteLine("cast of implicit " + c.V + " " + d);

        Many many = Many.Make(4);
        Console.WriteLine("many " + (Int32)many + " " + (Int64)many + " " + (String)many);

        // The arguments of a constructor, and of the constructor of the base class.
        Holder h = new Holder(7);
        Derived derived = new Derived(9);
        Console.WriteLine("constructor " + h.T.V + " " + derived.T.V);

        // The operands of the operators of the language: a Tagged is an Int32.
        Tagged five = 5;
        Int32 sum = five + 1;
        Int32 diff = 10 - five;
        Boolean eq = five == 5;
        Boolean less = five < 6;
        Int32 neg = -five;
        Int64 wide = five * 3L;
        Console.WriteLine("operators " + sum + " " + diff + " " + eq + " " + less + " " + neg + " " + wide);
        Console.WriteLine("done");
    }
}
