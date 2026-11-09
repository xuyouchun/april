// The conversions that an interface declares (static abstract explicit operator, static abstract implicit operator) are
// used through a generic parameter that has the interface as a constraint: (T)value, and a T that is converted to a
// type where a value of that type is wanted. A struct and a class implement them.

/* EXPECT

11 53
13 63
30 66
done

*/

import System;

using System;

interface IFromInt<TSelf> where TSelf : IFromInt<TSelf>
{
    static abstract explicit operator TSelf(Int32 value);

    static abstract implicit operator Int32(TSelf value);
}

struct Wide : IFromInt<Wide>
{
    public Int32 V;

    public static explicit operator Wide(Int32 value)
    {
        Wide w = new Wide();
        w.V = value * 2;
        return w;
    }

    public static implicit operator Int32(Wide value) { return value.V + 1; }
}

class Boxed : IFromInt<Boxed>
{
    public Int32 V;

    public static explicit operator Boxed(Int32 value)
    {
        Boxed b = new Boxed();
        b.V = value * 10;
        return b;
    }

    public static implicit operator Int32(Boxed value) { return value.V + 3; }
}

class Util
{
    public static T Make<T>(Int32 value) where T : IFromInt<T> { return (T)value; }

    public static Int32 Read<T>(T value) where T : IFromInt<T>
    {
        Int32 n = value;
        return n;
    }

    public static Int32 RoundTrip<T>(Int32 value) where T : IFromInt<T>
    {
        T t = (T)value;
        return Read(t);
    }

    public static Int32 Argument<T>(T value) where T : IFromInt<T> { return Twice(value); }

    static Int32 Twice(Int32 n) { return n * 2; }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Wide w = Util.Make<Wide>(5);
        Boxed b = Util.Make<Boxed>(5);
        Console.WriteLine(Util.Read(w) + " " + Util.Read(b));

        Console.WriteLine(Util.RoundTrip<Wide>(6) + " " + Util.RoundTrip<Boxed>(6));
        Console.WriteLine(Util.Argument(Util.Make<Wide>(7)) + " " + Util.Argument(Util.Make<Boxed>(3)));
        Console.WriteLine("done");
    }
}
