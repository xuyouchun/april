// A method is called on a value of a generic param that a member or an operator returns: T.Zero.ToString(),
// T.Abs(a).ToString(), (a + b).ToString(), (-a).ToString() (the value is the object of the call), for a number, a
// struct and a class.

/* EXPECT

0
5
8
11
-5
5
255
C7
C6
C-5
C12
K3
K10
K4
K-2

*/

import System;

using System;
using System.Numerics;

interface IZero<TSelf> where TSelf : IZero<TSelf>
{
    static abstract TSelf Zero { get; }

    static abstract TSelf Twice(TSelf value);

    static abstract TSelf operator +(TSelf left, TSelf right);

    static abstract TSelf operator -(TSelf value);
}

struct S : IZero<S>, IStringable
{
    public Int32 V;

    public static S Make(Int32 v)
    {
        S s = new S();
        s.V = v;
        return s;
    }

    public static S Zero { get { return Make(7); } }

    public static S Twice(S value) { return Make(value.V * 2); }

    public static S operator +(S left, S right) { return Make(left.V + right.V); }

    public static S operator -(S value) { return Make(-value.V); }

    public String ToString() { return "C" + V; }
}

class K : IZero<K>, IStringable
{
    public Int32 V;

    public static K Make(Int32 v)
    {
        K k = new K();
        k.V = v;
        return k;
    }

    public static K Zero { get { return Make(3); } }

    public static K Twice(K value) { return Make(value.V * 2); }

    public static K operator +(K left, K right) { return Make(left.V + right.V); }

    public static K operator -(K value) { return Make(-value.V); }

    public String ToString() { return "K" + V; }
}

class Util
{
    public static String Property<T>() where T : INumberBase<T>, IStringable { return T.Zero.ToString(); }

    public static String Variable<T>(T a) where T : INumberBase<T>, IStringable { return a.ToString(); }

    public static String Method<T>(T a) where T : INumberBase<T>, IStringable { return T.Abs(a).ToString(); }

    public static String Sum<T>(T a, T b) where T : INumberBase<T>, IStringable { return (a + b).ToString(); }

    public static String Minus<T>(T a) where T : INumberBase<T>, IStringable { return (-a).ToString(); }

    public static String Max<T>() where T : IMinMaxValue<T>, IStringable { return T.MaxValue.ToString(); }

    public static String UserProperty<T>() where T : IZero<T>, IStringable { return T.Zero.ToString(); }

    public static String UserMethod<T>(T a) where T : IZero<T>, IStringable { return T.Twice(a).ToString(); }

    public static String UserSum<T>(T a, T b) where T : IZero<T>, IStringable { return (a + b).ToString(); }

    public static String UserMinus<T>(T a) where T : IZero<T>, IStringable { return (-a).ToString(); }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine(Util.Property<Int32>());
        Console.WriteLine(Util.Variable(5));
        Console.WriteLine(Util.Method(-8));
        Console.WriteLine(Util.Sum(5, 6));
        Console.WriteLine(Util.Minus(5));
        Console.WriteLine(Util.Method((Int8)(-5)));
        Console.WriteLine(Util.Max<UInt8>());

        Console.WriteLine(Util.UserProperty<S>());
        Console.WriteLine(Util.UserMethod(S.Make(3)));
        Console.WriteLine(Util.UserMinus(S.Make(5)));
        Console.WriteLine(Util.UserSum(S.Make(5), S.Make(7)));

        Console.WriteLine(Util.UserProperty<K>());
        Console.WriteLine(Util.UserSum(K.Make(4), K.Make(6)));
        Console.WriteLine(Util.UserMethod(K.Make(2)));
        Console.WriteLine(Util.UserMinus(K.Make(2)));
    }
}
