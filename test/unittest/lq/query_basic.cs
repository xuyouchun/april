// The query expressions of lq in the code of C#: ``lq ends where the query ends (the analyzer of lq
// finds it), <%lq %> ends at %>. The expressions are those of C#, the clause keywords end them.

/* EXPECT

where 5,4,3
orderby 1,2,3,4,5
descending 5,4,3,2,1
select 10,50
typed 5,4
count 3
argument 10
subquery 5,4,3
bounded 5,1,4,2
lines 3,4
marker in text: ``lq from x in xs select x
done

*/

import System;

using System;
using System.Collections;
using System.Linq;

class Program
{
    static String Join(IEnumerable<Int32> items)
    {
        String text = "";
        foreach (Int32 item in items)
            text = text + (text == ""? "" : ",") + item;

        return text;
    }

    static Int32 Twice(Int32 value) => value * 2;

    [EntryPoint]
    public static void Main()
    {
        Int32[] xs = new Int32[] { 5, 1, 4, 2, 3 };

        Console.WriteLine("where " + Join(``lq from x in xs where x > 2 select x));
        Console.WriteLine("orderby " + Join(``lq from x in xs orderby x select x));
        Console.WriteLine("descending " + Join(``lq from x in xs orderby x descending select x));
        Console.WriteLine("select " + Join(``lq from x in new Int32[] { 1, 5 } select x * 10));
        Console.WriteLine("typed " + Join(``lq from Int32 x in xs where x > 3 select x));

        // A query in brackets is an expression: it has members.
        Console.WriteLine("count " + (``lq from x in xs where x > 2 select x).Count());

        // It ends at the bracket of the call.
        Console.WriteLine("argument " + Twice((``lq from x in xs where x == 5 select x).Sum()));

        // A query in a query.
        Int32[] ys = new Int32[] { 10, 20 };
        Console.WriteLine("subquery " + Join(``lq from x in xs where x > (``lq from y in ys select y).Count() select x));

        // The form with an end flag, in many lines, with comments.
        Console.WriteLine("bounded " + Join(<%lq from x in xs
                where x != 3   // not three
                select x %>));

        // The lines of the code are kept: the error of a later line is told at it (see errors).
        Console.WriteLine("lines " + Join((``lq from x in xs
                where x > 2
                where x < 5
                orderby x
                select x).Take(2)));

        // A mark in a string or in a comment is not a query.
        Console.WriteLine("marker in text: ``lq from x in xs select x"); // ``lq from x in xs select x
        Console.WriteLine("done");
    }
}
