// The names of the common delegates, that the library gives: Action<..> returns nothing, Func<TRet, ..> returns its first
// argument. They are aliases, the types are the Delegate<..> that they name. An alias of the program that has the name
// wins for the file.

/* EXPECT

act
3
3
s
12
True
file 5

*/

import System;

using System;

typedef Twice = Func<Int32, Int32>;

class Program
{
    static Int32 Add(Int32 a, Int32 b) { return a + b; }

    static Func<Int32, Int32> Doubler() { return x => x * 2; }

    [EntryPoint]
    public static void Main()
    {
        Action a = () => Console.WriteLine("act");
        a();

        Action<Int32> b = n => Console.WriteLine(n);
        b(3);

        Func<Int32, Int32, Int32> f = Add;
        Console.WriteLine(f(1, 2));

        Func<String> g = () => "s";
        Console.WriteLine(g());

        Console.WriteLine(Doubler()(6));

        Delegate<Int32, Int32, Int32> same = f;
        Console.WriteLine(typeof(Func<Int32, Int32, Int32>).Equals(typeof(Delegate<Int32, Int32, Int32>)));

        Twice t = x => x + 4;
        Console.WriteLine("file " + t(1));
    }
}
