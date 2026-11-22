// Raw strings: three quotes or more open and close the text, which has no escapes. When the quotes are alone in their lines the text is
// the lines between them, without the indentation of the closing quotes. $"""...""" has holes ($$ makes the holes of two braces).

/* EXPECT

raw "quoted" \n {x}
line one
  indented "two"
line three
[]
hello world
  x is 005
value {x} and 5
four """quotes""" inside
a {b} 5

*/

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Int32 x = 5;
        String name = "world";

        Console.WriteLine("""raw "quoted" \n {x}""");

        Console.WriteLine("""
            line one
              indented "two"
            line three
            """);

        Console.WriteLine("[" + """
            """ + "]");

        Console.WriteLine($"""
            hello {name}
              x is {x:D3}
            """);

        Console.WriteLine($$"""value {x} and {{x}}""");
        Console.WriteLine(""""four """quotes""" inside"""");
        Console.WriteLine($$"""a {b} {{x}}""");
    }
}
