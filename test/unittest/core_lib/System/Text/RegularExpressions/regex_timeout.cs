// Regex: the match timeout (a length of time): a search that takes more time is stopped with a RegexMatchTimeoutException.

/* EXPECT

timeout 00:00:00.1000000 0 (a+)+b 41 True True
property 00:00:00.1000000 0
infinite -10000 True -1
static timeout
matches timeout
quick True 2000 xb 2
fast False
done

*/

import System;

using System;
using System.Diagnostics;
using System.Text;
using System.Text.RegularExpressions;

class Program
{
    static String Repeat(String s, Int32 count)
    {
        StringBuilder sb = new StringBuilder();
        for (Int32 i = 0; i < count; i++)
            sb.Append(s);

        return sb.ToString();
    }

    [EntryPoint]
    public static void Main()
    {
        // The match timeout: a search that takes more time is stopped. The pattern takes 2 to the power of 40 steps: it would take
        // days, and the timeout is a tenth of a second.
        {
            String text = Repeat("a", 40) + "c";
            Regex timed = new Regex("(a+)+b", RegexOptions.None, TimeSpan.FromMilliseconds(100));
            Stopwatch watch = Stopwatch.StartNew();
            try
            {
                timed.IsMatch(text);
                Console.WriteLine("no timeout");
            }
            catch (RegexMatchTimeoutException e)
            {
                watch.Stop();
                Console.WriteLine("timeout " + e.MatchTimeout + " " + e.MaxSteps + " " + e.Pattern + " " + e.Input.Length + " " + (watch.ElapsedMilliseconds >= 90) + " " + (watch.ElapsedMilliseconds < 5000));
            }

            Console.WriteLine("property " + timed.MatchTimeout + " " + timed.MaxSteps);
            Console.WriteLine("infinite " + new Regex("a").MatchTimeout.Ticks + " " + (new Regex("a").MatchTimeout == Regex.InfiniteMatchTimeout) + " " + Regex.InfiniteMatchTimeout.TotalMilliseconds);

            // The timeout of each search, in the matches of a text, and of the static methods.
            try
            {
                Regex.IsMatch(text, "(a+)+b", RegexOptions.None, TimeSpan.FromMilliseconds(50));
                Console.WriteLine("no static timeout");
            }
            catch (RegexMatchTimeoutException)
            {
                Console.WriteLine("static timeout");
            }

            try
            {
                timed.Matches(text);
                Console.WriteLine("no matches timeout");
            }
            catch (RegexMatchTimeoutException)
            {
                Console.WriteLine("matches timeout");
            }

            // Searches that are short are not stopped, and the timeout is of one search: the matches of a long text, that take more than
            // the time together, are all found.
            Regex quick = new Regex("a+b", RegexOptions.None, TimeSpan.FromSeconds(30));
            Console.WriteLine("quick " + quick.IsMatch("aab") + " " + quick.Matches(Repeat("aab ", 2000)).Count + " " + Regex.Replace("aab", "a+", "x", RegexOptions.None, TimeSpan.FromSeconds(30)) + " " + Regex.Split("a-b", "-", RegexOptions.None, TimeSpan.FromSeconds(30)).Length);
            Console.WriteLine("fast " + new Regex("(?>a+)+b", RegexOptions.None, TimeSpan.FromMilliseconds(500)).IsMatch(text));
        }

        Console.WriteLine("done");
    }
}
