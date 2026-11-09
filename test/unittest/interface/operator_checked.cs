// operator checked: the operator that is used in a checked context (checked(a + b), a checked block, a checked
// statement group). A type that has no one of its own uses the operator in a checked context too. The interface can
// have a checked operator that is static virtual (it calls the one that is not checked), used through a generic
// parameter.

/* EXPECT

wrap 7 -2147483648
safe 7 overflow
plain 9 -2147483648
block overflow 9
generic 15 -2147483648 overflow
unary -5 -2147483648 overflow
done

*/

import System;

using System;

interface IAddOps<TSelf> where TSelf : IAddOps<TSelf>
{
    static abstract TSelf operator +(TSelf left, TSelf right);

    static virtual TSelf operator checked +(TSelf left, TSelf right) { return left + right; }
}

interface INegOps<TSelf> where TSelf : INegOps<TSelf>
{
    static abstract TSelf operator -(TSelf value);

    static virtual TSelf operator checked -(TSelf value) { return -value; }
}

// Both operators wrap.
struct Wrap : IAddOps<Wrap>, IStringable
{
    public Int32 V;

    public static Wrap Make(Int32 v)
    {
        Wrap w = new Wrap();
        w.V = v;
        return w;
    }

    public static Wrap operator +(Wrap left, Wrap right) { return Make(left.V + right.V); }

    public String ToString() { return "" + V; }
}

// The operator in a checked context raises.
struct Safe : IAddOps<Safe>, INegOps<Safe>, IStringable
{
    public Int32 V;

    public static Safe Make(Int32 v)
    {
        Safe s = new Safe();
        s.V = v;
        return s;
    }

    public static Safe operator +(Safe left, Safe right) { return Make(left.V + right.V); }

    public static Safe operator checked +(Safe left, Safe right) { return Make(checked(left.V + right.V)); }

    public static Safe operator -(Safe value) { return Make(-value.V); }

    public static Safe operator checked -(Safe value) { return Make(checked(-value.V)); }

    public String ToString() { return "" + V; }
}

class Util
{
    public static T Add<T>(T a, T b) where T : IAddOps<T> { return a + b; }

    public static T AddChecked<T>(T a, T b) where T : IAddOps<T> { return checked(a + b); }

    public static T Negate<T>(T a) where T : INegOps<T> { return -a; }

    public static T NegateChecked<T>(T a) where T : INegOps<T> { return checked(-a); }
}

class Program
{
    static String Try(Safe a, Safe b)
    {
        try
        {
            Safe r = checked(a + b);
            return r.ToString();
        }
        catch (OverflowException)
        {
            return "overflow";
        }
    }

    [EntryPoint]
    public static void Main()
    {
        Int32 max = 2147483647;

        // A type with one operator: the same in a checked context.
        Wrap w = Wrap.Make(max);
        Wrap w1 = checked(Wrap.Make(3) + Wrap.Make(4));
        Wrap w2 = checked(w + Wrap.Make(1));
        Console.WriteLine("wrap " + w1 + " " + w2);

        Safe s1 = Safe.Make(3) + Safe.Make(4);
        Console.WriteLine("safe " + s1 + " " + Try(Safe.Make(max), Safe.Make(1)));

        // Not checked: the operator that wraps.
        Safe s2 = Safe.Make(max) + Safe.Make(1);
        Safe s3 = Safe.Make(4) + Safe.Make(5);
        Console.WriteLine("plain " + s3 + " " + s2);

        String block = "no";
        checked
        {
            try
            {
                Safe s4 = Safe.Make(max) + Safe.Make(1);
                block = s4.ToString();
            }
            catch (OverflowException)
            {
                block = "overflow";
            }
        }

        Safe s5 = Safe.Make(4) + Safe.Make(5);
        Console.WriteLine("block " + block + " " + s5);

        // Through a generic parameter.
        Safe g1 = Util.AddChecked(Safe.Make(7), Safe.Make(8));
        Safe g2 = Util.Add(Safe.Make(max), Safe.Make(1));
        String g3;
        try
        {
            Safe g = Util.AddChecked(Safe.Make(max), Safe.Make(1));
            g3 = g.ToString();
        }
        catch (OverflowException)
        {
            g3 = "overflow";
        }

        Console.WriteLine("generic " + g1 + " " + g2 + " " + g3);

        Safe n1 = Util.NegateChecked(Safe.Make(5));
        Safe n2 = Util.Negate(Safe.Make(-2147483647 - 1));
        String n3;
        try
        {
            Safe n = Util.NegateChecked(Safe.Make(-2147483647 - 1));
            n3 = n.ToString();
        }
        catch (OverflowException)
        {
            n3 = "overflow";
        }

        Console.WriteLine("unary " + n1 + " " + n2 + " " + n3);
        Console.WriteLine("done");
    }
}
