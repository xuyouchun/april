// A typedef that has params: typedef Func<TRet, TArg> = Delegate<TRet, TArg, TArg>; . The params are replaced by the types that
// are written where it is used. The typedefs that have the same name and different counts of params are different typedefs,
// and a typedef of no params can have the name too.

/* EXPECT

hi
6
7
s
1 1
2
b
12

*/

import System;

using System;
using System.Collections;

typedef Action = Delegate<Void>;
typedef Func<TRet> = Delegate<TRet>;
typedef Func<TRet, TArg> = Delegate<TRet, TArg, TArg>;
typedef M = List<Int32>;
typedef M<T> = List<T>;
typedef M<K, V> = Dictionary<K, V>;
typedef Pair<T> = Dictionary<T, T>;
typedef Rows<T> = List<List<T>>;
typedef Twice<T> = M<M<T>>;

class Program
{
    static void Hi() { Console.WriteLine("hi"); }

    static Int32 Mul(Int32 a, Int32 b) { return a * b; }

    static Func<Int32, Int32> Adder() { return (a, b) => a + b; }

    [EntryPoint]
    public static void Main()
    {
        Action a = Hi;
        a();

        Func<Int32, Int32> f = Mul;
        Console.WriteLine(f(2, 3));

        Console.WriteLine(Adder()(3, 4));

        Func<String> g = () => "s";
        Console.WriteLine(g());

        M zero = new M();
        M<String> one = new M<String>();
        M<String, Int32> two = new M<String, Int32>();
        zero.Add(1);
        one.Add("x");
        two["a"] = 2;
        Console.WriteLine(zero.Count + " " + one.Count);

        Pair<Int32> p = new Pair<Int32>();
        p[1] = 2;
        Console.WriteLine(p[1]);

        Rows<String> rows = new Rows<String>();
        rows.Add(new List<String>());
        rows[0].Add("b");
        Console.WriteLine(rows[0][0]);

        Twice<Int32> t = new Twice<Int32>();
        t.Add(new M<Int32>());
        t[0].Add(12);
        Console.WriteLine(t[0][0]);
    }
}
