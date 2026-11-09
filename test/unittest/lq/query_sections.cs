// How the code of lq is found in the code of C#: <%lq ... %> is cut before it is analyzed, the
// start and the end flag of it are looked for by the language that has the code, so those that are
// in comments, strings and chars are not flags; ``lq ... ends where the query ends.

/* EXPECT

string: <%lq from x in xs select x %>
verbatim: <%lq from x in xs select x %>
bounded 1,2,3
end flag in a string 1%>,2%>,3%>
end flag in comments 1,2,3
two queries 2,4,6 2
in a query 3
two from 11,12,13
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

    static String JoinText(IEnumerable<String> items)
    {
        String text = "";
        foreach (String item in items)
            text = text + (text == ""? "" : ",") + item;

        return text;
    }

    [EntryPoint]
    public static void Main()
    {
        Int32[] xs = new Int32[] { 1, 2, 3 };
        Int32[] ys = new Int32[] { 10 };

        // A start flag in a comment: <%lq from x in xs select x
        /* and in another one: <%lq from x in xs
           select x */
        Console.WriteLine("string: <%lq from x in xs select x %>");
        Console.WriteLine(@"verbatim: <%lq from x in xs select x %>");

        Console.WriteLine("bounded " + Join(<%lq from x in xs select x %>));
        Console.WriteLine("end flag in a string " + JoinText(<%lq from x in xs select x + "%>" %>));
        Console.WriteLine("end flag in comments " + Join(<%lq from x in xs /* %> */ select x // %>
                                                         %>));
        Console.WriteLine("two queries " + Join(<%lq from x in xs select x * 2 %>) + " "
                                         + (<%lq from x in xs where x > 1 select x %>).Count());
        Console.WriteLine("in a query " + (<%lq from x in xs where x > (``lq from y in xs where y < 3 select y).Count() select x %>).Sum());
        Console.WriteLine("two from " + Join(<%lq from x in xs from y in ys select x + y %>));
        Console.WriteLine("done");
    }
}
