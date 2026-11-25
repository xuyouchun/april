// new { A = 1, B = "x" } is an anonymous type: the members are read by their names, var keeps them. (It is a tuple that has the
// names: it is a struct that is compared by its members.)

/* EXPECT

1 x
7
same True
other False
list 3
nested 5 inner
text (1, x)

*/

import System;

using System;
using System.Collections;

class Program
{
    static Int32 Total(Object o) { return 0; }

    [EntryPoint]
    public static void Main()
    {
        var item = new { A = 1, B = "x" };
        Console.WriteLine(item.A + " " + item.B);

        var single = new { Only = 7 };
        Console.WriteLine(single.Only);

        var p = new { A = 1, B = "x" };
        var q = new { A = 1, B = "x" };
        var r = new { A = 2, B = "x" };
        Console.WriteLine("same " + p.Equals(q));
        Console.WriteLine("other " + p.Equals(r));

        List<int> numbers = new List<int> { 1, 2, 3 };
        var summary = new { Count = numbers.Count, First = numbers[0] };
        Console.WriteLine("list " + summary.Count);

        var nested = new { Id = 5, Inner = new { Name = "inner" } };
        Console.WriteLine("nested " + nested.Id + " " + nested.Inner.Name);

        Console.WriteLine("text " + item);
    }
}
