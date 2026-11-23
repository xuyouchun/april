// The last param of an alias can be a pack (TArgs...): it is all the arguments that follow the others, none too. One alias is
// all the counts of arguments. An alias that has as many params and no pack wins for its count, and the aliases that have a
// pack are chosen by the params that they have before it.

/* EXPECT

5
1
n4
act
pack 9
fixed 3
two 7
seven 8

*/

import System;

using System;
using System.Collections;

typedef Func<TRet, TArgs...> = Delegate<TRet, TArgs...>;
typedef Action<TArgs...> = Delegate<Void, TArgs...>;
typedef Pick<TRet, TArg> = Delegate<TRet, TArg, TArg>;
typedef Tuple2<T, TRest...> = Dictionary<T, TRest...>;

class Program
{
    static Int32 Add(Int32 a, Int32 b) { return a + b; }
    static Int32 One() { return 1; }
    static void Show(String s, Int32 n) { Console.WriteLine(s + n); }
    static Int32 Three(Int32 a, Int32 b, Int32 c) { return a + b + c; }

    [EntryPoint]
    public static void Main()
    {
        Func<Int32, Int32, Int32> f = Add;
        Console.WriteLine(f(2, 3));

        Func<Int32> g = One;
        Console.WriteLine(g());

        Action<String, Int32> h = Show;
        h("n", 4);

        Action a = () => Console.WriteLine("act");
        a();

        Func<Int32, Int32, Int32, Int32> three = Three;
        Console.WriteLine("pack " + three(2, 3, 4));

        Pick<Int32, Int32> p = Add;
        Console.WriteLine("fixed " + p(1, 2));

        Tuple2<Int32, Int32> d = new Tuple2<Int32, Int32>();
        d[2] = 7;
        Console.WriteLine("two " + d[2]);

        Action<Int32> act1 = n => Console.WriteLine("seven " + (n + 1));
        act1(7);
    }
}
