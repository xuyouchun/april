// An alias of a nested type of a generic type: the params of the alias are the arguments of the host and of the nested type,
// wherever they are written, and the other types of the name are those of the program.

/* EXPECT

1 a 2
5 7 b
9
x
6
4
True

*/

import System;

using System;

class Outer<T1>
{
    public class Inner<T2, T3>
    {
        public T1 A;
        public T2 B;
        public T3 C;

        public static String Name() { return "x"; }
    }

    public class Plain
    {
        public T1 Value;
    }
}

class Fixed
{
    public class Leaf<T>
    {
        public T Value;
    }
}

typedef Class3<T1, T2, T3> = Outer<T1>.Inner<T2, T3>;
typedef Class4<T1> = Outer<Int32>.Inner<T1, Int64>;
typedef Class5<T> = Outer<T>.Plain;
typedef Class6 = Outer<String>.Plain;
typedef Class7<T> = Fixed.Leaf<T>;
typedef Chain<T> = Class3<Int32, T, T>;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Class3<Int32, String, Int64> a = new Class3<Int32, String, Int64>();
        a.A = 1; a.B = "a"; a.C = 2;
        Console.WriteLine(a.A + " " + a.B + " " + a.C);

        Class4<String> b = new Class4<String>();
        b.A = 5; b.B = "b"; b.C = 7;
        Console.WriteLine(b.A + " " + b.C + " " + b.B);

        Class5<Int32> p = new Class5<Int32>();
        p.Value = 9;
        Console.WriteLine(p.Value);

        Console.WriteLine(Class3<Int32, Int32, Int32>.Name());

        Class6 q = new Class6();
        q.Value = "s";
        Class7<Int32> leaf = new Class7<Int32>();
        leaf.Value = 6;
        Console.WriteLine(leaf.Value);

        Chain<Int32> c = new Chain<Int32>();
        c.A = 1; c.B = 1; c.C = 2;
        Console.WriteLine(c.A + c.B + c.C);

        Outer<Int32>.Inner<String, Int64> same = b;
        Console.WriteLine(typeof(Class4<String>).Equals(typeof(Outer<Int32>.Inner<String, Int64>)));
    }
}
