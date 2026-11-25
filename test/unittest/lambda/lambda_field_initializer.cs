// A lambda can be the value of a field: it is a static method of the type, that uses no variable (a field initializer has no
// local variables, and no object).

/* EXPECT

2
8
say
5
9

*/

import System;

using System;

class Holder
{
    public Func<Int32, Int32> Inc = x => x + 1;
    public static Func<Int32, Int32> Twice = x => x * 2;
    public Action Say = () => { Console.WriteLine("say"); };
    public Func<Int32, Int32, Int32> Sum { get; set; } = (a, b) => a + b;
}

class Program
{
    static Func<Int32> Const = () => 9;

    [EntryPoint]
    public static void Main()
    {
        Holder h = new Holder();
        Console.WriteLine(h.Inc(1));
        Console.WriteLine(Holder.Twice(4));
        h.Say();
        var sum = h.Sum;
        Console.WriteLine(sum(2, 3));
        Console.WriteLine(Const());
    }
}
