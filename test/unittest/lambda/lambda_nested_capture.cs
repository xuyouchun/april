// A lambda in a lambda uses the variables of the method, and those of the outer lambda; implicit params too.

/* EXPECT

12
17
17
725
11
7

*/

import System;

using System;

class Calc
{
    // The inner lambda uses the param of the method, and the param of the outer one.
    public static Delegate<Delegate<Int32, Int32>, Int32> Curry(Int32 a)
    {
        return b => c => a * 100 + b * 10 + c;
    }

    // The inner lambda uses the variable of the outer lambda.
    public static Delegate<Int32, Int32> Twice(Int32 k)
    {
        return x =>
        {
            int y = x + k;
            Delegate<Int32, Int32> inner = z => z * y + k;
            return inner(2);
        };
    }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        int a = 3;
        int b = 4;

        // The inner lambda uses the variables of the method.
        Delegate<Delegate<Int32>> outer = () => () => a * b;
        Delegate<Int32> inner = outer();
        Console.WriteLine(inner());

        // The inner lambda uses a variable that the outer one does not use.
        Delegate<Int32, Int32> add = x =>
        {
            Delegate<Int32, Int32> h = y => y + a;
            return h(x) + b;
        };
        Console.WriteLine(add(10));

        // The inner lambda is a param of the outer one, and the variables of the method.
        Delegate<Int32, Int32> f = x =>
        {
            Delegate<Int32, Int32> g = y => x * y + a + b;
            return g(5);
        };
        Console.WriteLine(f(2));

        var c = Calc.Curry(7);
        var c2 = c(2);
        Console.WriteLine(c2(5));

        var t = Calc.Twice(1);
        Console.WriteLine(t(4));

        // Three levels.
        Delegate<Delegate<Delegate<Int32>>> three = () => () => () => a + b;
        Console.WriteLine(three()()());
    }
}
