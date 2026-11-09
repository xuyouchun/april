// The primitive types have members that a box of them calls by an interface or by a virtual call
// (IComparable<T>, IEquatable<T>, IEquality, ToString): the receiver is the value. A
// value of a generic param is passed to a reference param, and is joined to a String.

/* EXPECT

direct -1 True 4
interface -1 1 True
object 4 4 True False
string -1 True
generic a5 ab 2.5
probe cmp1 eq False
probe cmp0 eq True
done

*/

import System;

using System;

class Program
{
    static String Join<T>(String text, T item) => text + item;

    static String Kind(Object o) => Object.ToString(o);

    static String Hold<T>(T item) => Kind(item);

    static String Probe<T>(T x, T y)
    {
        Object a = x;
        IComparable<T> comparable = a as IComparable<T>;
        IEquatable<T> equatable = a as IEquatable<T>;

        return "cmp" + comparable.CompareTo(y) + " eq " + equatable.Equals(y);
    }

    [EntryPoint]
    public static void Main()
    {
        Int32 a = 4;
        Console.WriteLine("direct " + a.CompareTo(6) + " " + a.Equals(4) + " " + a.GetHashCode());

        IComparable<Int32> comparable = a;
        IEquatable<Int32> equatable = a;
        Console.WriteLine("interface " + comparable.CompareTo(6) + " " + comparable.CompareTo(2)
                                      + " " + equatable.Equals(4));

        Object o = a;
        Console.WriteLine("object " + Object.HashOf(o) + " " + Object.ToString(o) + " " + Object.Equals(o, 4)
                                   + " " + Object.Equals(o, "4"));

        String s = "abc";
        IComparable<String> text = s;
        Console.WriteLine("string " + text.CompareTo("abd") + " " + s.Equals("abc"));

        Console.WriteLine("generic " + Join<Int32>("a", 5) + " " + Join<String>("a", "b")
                                     + " " + Hold<Double>((Double)2.5));

        Console.WriteLine("probe " + Probe<Int32>(5, 3));
        Console.WriteLine("probe " + Probe<Int64>((Int64)7, (Int64)7));
        Console.WriteLine("done");
    }
}
