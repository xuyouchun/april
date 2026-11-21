// TimeSpan: a length of time in ticks of 100 nanoseconds: the constructors, the parts, the totals, FromX, the arithmetic and the
// operators, the comparison, the text, the limits and the overflow.

/* EXPECT

ctor 01:02:03 | 1.02:03:04 | 1.02:03:04.0050000 | 00:00:00 | 00:00:00.0000001
from 1.12:00:00 | 1.01:00:00 | 01:30:00 | 00:00:00.5000000 | 00:00:01.5000000 | 00:00:00.0000001
negative -00:01:30 | -2.12:00:00 | -00:00:00.0000001 | -00:00:01.5000000
parts 0 1 1 1 500 36615000000
totals 3661.5 3661500 1.5 1.5 0.5
signs -3 -12 0 0 -7 -3.5000000810185186
arith 00:02:00 | 00:01:00 | -00:01:00 | -00:01:30 | 00:01:00 | 00:02:15 | 00:00:22.5000000 | 3
operators 00:02:00 | 00:01:00 | -00:01:30 | 00:01:30 | 00:03:00 | 00:00:45 | 00:00:30 | 3
compare True False True True True True 1 -1 0
equals True False True False True True
limits 9223372036854775807 -9223372036854775808 0
text 10675199.02:48:05.4775807 | -10675199.02:48:05.4775808
constants 10000 10000000 600000000 36000000000 864000000000
cut 1 -1 5000 0
max plus overflow
min minus overflow
negate min overflow
duration min overflow
days overflow
seconds overflow
nan argument
hours overflow
multiply overflow
ok no error 00:00:00
done

*/

import System;

using System;

class Program
{
    static void Overflow(String label, Delegate<TimeSpan> f)
    {
        try
        {
            TimeSpan t = f();
            Console.WriteLine(label + " no error " + t);
        }
        catch (OverflowException)
        {
            Console.WriteLine(label + " overflow");
        }
        catch (ArgumentException)
        {
            Console.WriteLine(label + " argument");
        }
    }

    [EntryPoint]
    public static void Main()
    {
        // The constructors and the text.
        Console.WriteLine("ctor " + new TimeSpan(1, 2, 3) + " | " + new TimeSpan(1, 2, 3, 4) + " | " + new TimeSpan(1, 2, 3, 4, 5) + " | " + new TimeSpan(0) + " | " + new TimeSpan(1));
        Console.WriteLine("from " + TimeSpan.FromDays(1.5) + " | " + TimeSpan.FromHours(25) + " | " + TimeSpan.FromMinutes(90) + " | " + TimeSpan.FromSeconds(0.5) + " | " + TimeSpan.FromMilliseconds(1500) + " | " + TimeSpan.FromTicks(1));
        Console.WriteLine("negative " + TimeSpan.FromSeconds(-90) + " | " + TimeSpan.FromDays(-2.5) + " | " + new TimeSpan(-1) + " | " + TimeSpan.FromMilliseconds(-1500));

        // The parts and the totals.
        TimeSpan t = TimeSpan.FromSeconds(3661.5);
        Console.WriteLine("parts " + t.Days + " " + t.Hours + " " + t.Minutes + " " + t.Seconds + " " + t.Milliseconds + " " + t.Ticks);
        Console.WriteLine("totals " + t.TotalSeconds + " " + t.TotalMilliseconds + " " + TimeSpan.FromHours(36).TotalDays + " " + TimeSpan.FromMinutes(90).TotalHours + " " + TimeSpan.FromSeconds(30).TotalMinutes);
        TimeSpan n = TimeSpan.FromDays(-3.5) - TimeSpan.FromMilliseconds(7);
        Console.WriteLine("signs " + n.Days + " " + n.Hours + " " + n.Minutes + " " + n.Seconds + " " + n.Milliseconds + " " + n.TotalDays);

        // The arithmetic.
        TimeSpan a = TimeSpan.FromSeconds(90);
        TimeSpan b = TimeSpan.FromSeconds(30);
        Console.WriteLine("arith " + a.Add(b) + " | " + a.Subtract(b) + " | " + b.Subtract(a) + " | " + a.Negate() + " | " + b.Subtract(a).Duration() + " | " + a.Multiply(1.5) + " | " + a.Divide(4) + " | " + a.Divide(b));
        Console.WriteLine("operators " + (a + b) + " | " + (a - b) + " | " + (-a) + " | " + (+a) + " | " + a * 2 + " | " + 0.5 * a + " | " + a / 3 + " | " + a / b);

        // The comparison.
        Console.WriteLine("compare " + (a > b) + " " + (a < b) + " " + (a >= a) + " " + (b <= a) + " " + (a == TimeSpan.FromSeconds(90)) + " " + (a != b) + " " + a.CompareTo(b) + " " + b.CompareTo(a) + " " + a.CompareTo(a));
        Object boxedB = b;
        Object boxedA = TimeSpan.FromSeconds(90);
        Console.WriteLine("equals " + a.Equals(TimeSpan.FromMilliseconds(90000)) + " " + a.Equals(boxedB) + " " + a.Equals(boxedA) + " " + a.Equals("x") + " " + (a.GetHashCode() == TimeSpan.FromSeconds(90).GetHashCode()) + " " + (TimeSpan.Zero == new TimeSpan(0)));

        // The limits.
        Console.WriteLine("limits " + TimeSpan.MaxValue.Ticks + " " + TimeSpan.MinValue.Ticks + " " + TimeSpan.Zero.Ticks);
        Console.WriteLine("text " + TimeSpan.MaxValue + " | " + TimeSpan.MinValue);
        Console.WriteLine("constants " + TimeSpan.TicksPerMillisecond + " " + TimeSpan.TicksPerSecond + " " + TimeSpan.TicksPerMinute + " " + TimeSpan.TicksPerHour + " " + TimeSpan.TicksPerDay);

        // The cut of the part of a tick, and the sign.
        Console.WriteLine("cut " + TimeSpan.FromSeconds(0.00000019).Ticks + " " + TimeSpan.FromSeconds(-0.00000019).Ticks + " " + TimeSpan.FromMilliseconds(0.5).Ticks + " " + TimeSpan.FromDays(0).Ticks);

        // The errors.
        Overflow("max plus", () => TimeSpan.MaxValue + new TimeSpan(1));
        Overflow("min minus", () => TimeSpan.MinValue - new TimeSpan(1));
        Overflow("negate min", () => -TimeSpan.MinValue);
        Overflow("duration min", () => TimeSpan.MinValue.Duration());
        Overflow("days", () => TimeSpan.FromDays(1e10));
        Overflow("seconds", () => TimeSpan.FromSeconds(-1e30));
        Overflow("nan", () => TimeSpan.FromSeconds(0.0 / 0.0));
        Overflow("hours", () => new TimeSpan(2147483647, 0, 0));
        Overflow("multiply", () => TimeSpan.MaxValue * 2);
        Overflow("ok", () => TimeSpan.MaxValue - TimeSpan.MaxValue);

        Console.WriteLine("done");
    }
}
