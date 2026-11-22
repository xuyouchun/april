// Interpolated strings: $"...{expression[,alignment][:format]}...", $@"..." and @$"..." (verbatim), {{ and }} are the braces.
// Each one is String.Format of the text and of the expressions.

/* EXPECT

Error: boom
hello world, x = 5, sum = 6
{literal} 5
padded [    5] [5    ] fixed 3.14 005
verbatim "world" \n 5
also "world" \n 5
call 5-world big
no holes
escapes: 5 "
nested 7 abc
list 3 2
done

*/

import System;

using System;
using System.Collections;

class Program
{
    static Int32 Twice(Int32 value) => value * 2;

    [EntryPoint]
    public static void Main()
    {
        Int32 x = 5;
        String name = "world";
        Exception ex = new InvalidOperationException("boom");

        Console.WriteLine($"Error: {ex.Message}");
        Console.WriteLine($"hello {name}, x = {x}, sum = {x + 1}");
        Console.WriteLine($"{{literal}} {x}");
        Console.WriteLine($"padded [{x,5}] [{x,-5}] fixed {3.14159:F2} {x:D3}");
        Console.WriteLine($@"verbatim ""{name}"" \n {x}");
        Console.WriteLine(@$"also ""{name}"" \n {x}");
        Console.WriteLine($"call {String.Format("{0}-{1}", x, name)} {(x > 3 ? "big" : "small")}");
        Console.WriteLine($"no holes");
        Console.WriteLine($"escapes: {x} \"");

        // The expressions are any: calls, strings in the hole, a hole in a hole.
        Console.WriteLine($"nested {Twice(x) - 3} {$"{name.Substring(0, 0)}abc"}");

        List<Int32> list = new List<Int32>();
        list.Add(1);
        list.Add(2);
        list.Add(3);
        Console.WriteLine($"list {list.Count} {list[1]}");

        String empty = $"";
        Console.WriteLine(empty.Length == 0? "done" : "not empty");
    }
}
