// The items of a tuple whose types are generic params of the method that reads them (t.Item2 of a Tuple<T, T>), for a
// number, a string and a class, and the same method with other arguments: the method that is found by a
// reference whose types are those of the caller is not kept for the other callers.

/* EXPECT

4 b 7
3 x 9
swapped 4 3
swapped y x
first 10 second 20
done

*/

import System;

using System;

class Box
{
    public Int32 V;

    public static Box Make(Int32 v)
    {
        Box b = new Box();
        b.V = v;
        return b;
    }
}

interface IPair<TSelf> where TSelf : IPair<TSelf>
{
    static abstract Tuple<TSelf, TSelf> Split(TSelf value);
}

class Num : IPair<Num>
{
    public Int32 V;

    public static Num Make(Int32 v)
    {
        Num n = new Num();
        n.V = v;
        return n;
    }

    public static Tuple<Num, Num> Split(Num value) { return new Tuple<Num, Num>(Make(value.V / 2), Make(value.V - value.V / 2)); }
}

class Util
{
    public static T Second<T>(T a, T b)
    {
        Tuple<T, T> t = new Tuple<T, T>(a, b);
        return t.Item2;
    }

    public static T First<T>(Tuple<T, T> t) { return t.Item1; }

    public static Tuple<T, T> Swap<T>(Tuple<T, T> t)
    {
        T x = t.Item1;
        T y = t.Item2;
        return new Tuple<T, T>(y, x);
    }

    public static T Larger<T>(T value) where T : IPair<T>
    {
        Tuple<T, T> parts = T.Split(value);
        T half = parts.Item2;
        return half;
    }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine(Util.Second(3, 4) + " " + Util.Second("a", "b") + " " + Util.Second(Box.Make(6), Box.Make(7)).V);
        Console.WriteLine(Util.First(new Tuple<Int32, Int32>(3, 4)) + " " + Util.First(new Tuple<String, String>("x", "y")) + " "
            + Util.First(new Tuple<Box, Box>(Box.Make(9), Box.Make(1))).V);

        Tuple<Int32, Int32> s1 = Util.Swap(new Tuple<Int32, Int32>(3, 4));
        Tuple<String, String> s2 = Util.Swap(new Tuple<String, String>("x", "y"));
        Console.WriteLine("swapped " + s1.Item1 + " " + s1.Item2);
        Console.WriteLine("swapped " + s2.Item1 + " " + s2.Item2);

        Num big = Util.Larger(Num.Make(20));
        Num small = Util.Larger(Num.Make(21));
        Console.WriteLine("first " + big.V + " second " + (small.V + 9));
        Console.WriteLine("done");
    }
}
