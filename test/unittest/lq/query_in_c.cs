// A query of lq in the code of c: <%c { ... } %> has ``lq and <%lq ... %> in its statements (c is cs with the pointers,
// the code of lq goes on in it as it does in cs), and the query reads what the pointers wrote.

/* EXPECT

sum 10
query 4,3,2
pointers 8 5
nested 1 12
done

*/

import System;

using System;
using System.Collections;
using System.Linq;
using System.Runtime;

class Program
{
    static String Join(IEnumerable<Int32> items)
    {
        String text = "";
        foreach (Int32 item in items)
            text = text + (text == ""? "" : ",") + item;

        return text;
    }

    [EntryPoint]
    public static void Main()
    {
        Int32[] xs = new Int32[5];

        <%c
        {
            // The array is written with a pointer.
            int* p = (int*)Ptr.AddressOf(xs);
            for (int i = 0; i < 5; i++)
                p[i] = i;

            int total = 0;
            for (int i = 0; i < 5; i++)
                total += *(p + i);

            Console.WriteLine("sum " + total);

            // ``lq ends where the query ends, in the code of c.
            Console.WriteLine("query " + Join(``lq from x in xs where x > 1 orderby x descending select x));

            // The query has a count that c uses.
            int count = (<%lq from x in xs where x % 2 == 0 select x %>).Count();
            int* q = &count;
            *q = *q * 2;
            Console.WriteLine("pointers " + (*q + 2) + " " + (*q - 1));

            // The variables of the code of c are the ones of the query (lq has the syntax of cs, not the one of c).
            Int32 shift = 10;
            Console.WriteLine("nested " + (<%lq from x in xs where x == 2 select x %>).Count() + " "
                + (<%lq from x in xs where x == 2 select x + shift %>).First());
        }
        %>

        Console.WriteLine("done");
    }
}
