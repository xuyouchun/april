// Regex: the timeouts that are not valid for the constructor: a length of time that is zero or negative (but the infinite one) or more than
// 24 days is an OutOfRangeException.

/* EXPECT

timeouts invalid invalid invalid valid valid
done

*/

import System;

using System;
using System.Text.RegularExpressions;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        // The timeouts that are not valid.
        {
            String[] results = new String[5];
            TimeSpan[] values = new TimeSpan[] { TimeSpan.Zero, TimeSpan.FromMilliseconds(-5), TimeSpan.FromDays(25), TimeSpan.FromDays(24), TimeSpan.FromMilliseconds(-1) };
            for (Int32 i = 0; i < values.Length; i++)
            {
                try
                {
                    new Regex("a", RegexOptions.None, values[i]);
                    results[i] = "valid";
                }
                catch (OutOfRangeException)
                {
                    results[i] = "invalid";
                }
            }

            Console.WriteLine("timeouts " + results[0] + " " + results[1] + " " + results[2] + " " + results[3] + " " + results[4]);
        }

        Console.WriteLine("done");
    }
}
