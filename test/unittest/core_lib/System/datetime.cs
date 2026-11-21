// DateTime: a point of time in UTC (ticks since 0001-01-01): the constructors, the parts, the calendar, the arithmetic, the
// comparison, the text, the limits, the errors, and the clock of the system.

/* EXPECT

parts 2026-11-21 13:45:30.250 6 325 639308655302500000
day of week 6 True 0 6
date 2026-11-21T00:00:00.0000000Z | 13:45:30.2500000 | 2026-11-21T00:00:00.0000000Z | 2026-11-21T01:02:03.0000000Z
text 11/21/2026 13:45:30 | 11/21/2026 13:45:30 | 11/21/2026 | 13:45:30 | 2026-11-21T13:45:30 | 2026-11-21T13:45:30.2500000Z | 11/21/2026 13:45:30 | 11/21/2026 13:45:30
early 0005-03-07T09:04:02.0000000Z
min 0001-01-01T00:00:00.0000000Z 0 1
max 9999-12-31T23:59:59.9999999Z 3155378975999999999 5 365
epoch 1970-01-01T00:00:00.0000000Z 621355968000000000 4
leap False True True False False True False
days in month 29 28 30 31 28
known days 3 1 4 4 366 365
calendar 73414 0 2101-01-01T00:00:00
range 0
add 2026-02-01T22:00:00 | 2026-01-30T23:00:00 | 2026-01-31T11:30:00 | 2026-02-01T10:00:00 | 2026-01-31T10:00:01.5000000Z | 2026-01-31T10:00:00.0000001Z | 2026-03-02T10:00:00 | 2025-12-31T10:00:00
months 2026-02-28T10:00:00 | 2024-02-29T00:00:00 | 2026-02-28T00:00:00 | 2026-01-31T10:00:00 | 2027-01-31T10:00:00 | 2024-12-31T10:00:00 | 2026-12-31T10:00:00
years 2025-02-28T00:00:00 | 2028-02-29T00:00:00 | 2000-02-29T00:00:00 | 2126-01-31T10:00:00
difference 33.12:00:00 | 365 | -1.00:00:00 | 2026-02-01T00:00:00 | 2026-01-31T00:00:00
compare True False False True True True -1 1 0 True True False True
errors out of range | out of range | out of range | out of range | out of range | out of range | out of range | out of range
limits out of range | out of range | out of range | out of range | out of range | out of range | overflow | 9999-12-31T23:59:59.9999999Z
format error
year error
clock True True True True True True
done

*/

import System;

using System;

class Program
{
    static String Try(Delegate<DateTime> f)
    {
        try
        {
            return f().ToString("O");
        }
        catch (OutOfRangeException)
        {
            return "out of range";
        }
        catch (OverflowException)
        {
            return "overflow";
        }
    }

    static String Parts(DateTime t)
    {
        return t.Year + "-" + t.Month + "-" + t.Day + " " + t.Hour + ":" + t.Minute + ":" + t.Second + "." + t.Millisecond + " " + (Int32)t.DayOfWeek + " " + t.DayOfYear;
    }

    [EntryPoint]
    public static void Main()
    {
        // A date and its parts.
        DateTime t = new DateTime(2026, 11, 21, 13, 45, 30, 250);
        Console.WriteLine("parts " + Parts(t) + " " + t.Ticks);
        Console.WriteLine("day of week " + (Int32)t.DayOfWeek + " " + (t.DayOfWeek == DayOfWeek.Saturday) + " " + (Int32)DayOfWeek.Sunday + " " + (Int32)DayOfWeek.Saturday);
        Console.WriteLine("date " + t.Date.ToString("O") + " | " + t.TimeOfDay + " | " + new DateTime(2026, 11, 21).ToString("O") + " | " + new DateTime(2026, 11, 21, 1, 2, 3).ToString("O"));

        // The text.
        Console.WriteLine("text " + t + " | " + t.ToString("G") + " | " + t.ToString("d") + " | " + t.ToString("T") + " | " + t.ToString("s") + " | " + t.ToString("O") + " | " + t.ToString(null) + " | " + t.ToString(""));
        Console.WriteLine("early " + new DateTime(5, 3, 7, 9, 4, 2).ToString("O"));

        // The limits and the epoch.
        Console.WriteLine("min " + DateTime.MinValue.ToString("O") + " " + DateTime.MinValue.Ticks + " " + (Int32)DateTime.MinValue.DayOfWeek);
        Console.WriteLine("max " + DateTime.MaxValue.ToString("O") + " " + DateTime.MaxValue.Ticks + " " + (Int32)DateTime.MaxValue.DayOfWeek + " " + DateTime.MaxValue.DayOfYear);
        Console.WriteLine("epoch " + DateTime.UnixEpoch.ToString("O") + " " + DateTime.UnixEpoch.Ticks + " " + (Int32)DateTime.UnixEpoch.DayOfWeek);

        // The calendar.
        Console.WriteLine("leap " + DateTime.IsLeapYear(1900) + " " + DateTime.IsLeapYear(2000) + " " + DateTime.IsLeapYear(2024) + " " + DateTime.IsLeapYear(2100) + " " + DateTime.IsLeapYear(2026) + " " + DateTime.IsLeapYear(4) + " " + DateTime.IsLeapYear(1));
        Console.WriteLine("days in month " + DateTime.DaysInMonth(2024, 2) + " " + DateTime.DaysInMonth(2023, 2) + " " + DateTime.DaysInMonth(2026, 4) + " " + DateTime.DaysInMonth(2026, 12) + " " + DateTime.DaysInMonth(1900, 2));
        Console.WriteLine("known days " + (Int32)new DateTime(2000, 3, 1).DayOfWeek + " " + (Int32)new DateTime(1900, 1, 1).DayOfWeek + " " + (Int32)new DateTime(1970, 1, 1).DayOfWeek + " " + (Int32)new DateTime(2024, 2, 29).DayOfWeek + " " + new DateTime(2024, 12, 31).DayOfYear + " " + new DateTime(2023, 12, 31).DayOfYear);

        // The date of every day is the same when it is read back: the parts of the days of 1900 to 2100, and the weekday goes on
        // one by one.
        Int32 bad = 0;
        DateTime day = new DateTime(1900, 1, 1);
        Int32 weekday = (Int32)day.DayOfWeek;
        Int32 count = 0;
        for (Int32 year = 1900; year <= 2100; year++)
        {
            for (Int32 month = 1; month <= 12; month++)
            {
                Int32 last = DateTime.DaysInMonth(year, month);
                for (Int32 d = 1; d <= last; d++)
                {
                    DateTime x = new DateTime(year, month, d);
                    if (x.Year != year || x.Month != month || x.Day != d || x != day || (Int32)x.DayOfWeek != weekday)
                        bad++;

                    day = day.AddDays(1);
                    weekday = (weekday + 1) % 7;
                    count++;
                }
            }
        }

        Console.WriteLine("calendar " + count + " " + bad + " " + day.ToString("s"));

        // The years of the whole range, the first and the last day of each month.
        Int32 bad2 = 0;
        for (Int32 year = 1; year <= 9999; year += 61)
        {
            for (Int32 month = 1; month <= 12; month++)
            {
                DateTime first = new DateTime(year, month, 1);
                DateTime end = new DateTime(year, month, DateTime.DaysInMonth(year, month), 23, 59, 59, 999);
                if (first.Year != year || first.Month != month || first.Day != 1 || end.Year != year || end.Month != month || end.Day != DateTime.DaysInMonth(year, month) || end.Hour != 23)
                    bad2++;
            }
        }

        Console.WriteLine("range " + bad2);

        // The arithmetic.
        DateTime a = new DateTime(2026, 1, 31, 10, 0, 0);
        Console.WriteLine("add " + a.AddDays(1.5).ToString("s") + " | " + a.AddHours(-11).ToString("s") + " | " + a.AddMinutes(90).ToString("s") + " | " + a.AddSeconds(86400).ToString("s") + " | " + a.AddMilliseconds(1500).ToString("O") + " | " + a.AddTicks(1).ToString("O") + " | " + a.Add(TimeSpan.FromDays(30)).ToString("s") + " | " + a.Subtract(TimeSpan.FromDays(31)).ToString("s"));
        Console.WriteLine("months " + a.AddMonths(1).ToString("s") + " | " + new DateTime(2024, 1, 31).AddMonths(1).ToString("s") + " | " + new DateTime(2026, 3, 31).AddMonths(-1).ToString("s") + " | " + a.AddMonths(0).ToString("s") + " | " + a.AddMonths(12).ToString("s") + " | " + a.AddMonths(-13).ToString("s") + " | " + a.AddMonths(11).ToString("s"));
        Console.WriteLine("years " + new DateTime(2024, 2, 29).AddYears(1).ToString("s") + " | " + new DateTime(2024, 2, 29).AddYears(4).ToString("s") + " | " + new DateTime(2024, 2, 29).AddYears(-24).ToString("s") + " | " + a.AddYears(100).ToString("s"));
        TimeSpan span = new DateTime(2026, 12, 25) - new DateTime(2026, 11, 21, 12, 0, 0);
        Console.WriteLine("difference " + span + " | " + (new DateTime(2026, 1, 1) - new DateTime(2025, 1, 1)).TotalDays + " | " + new DateTime(2026, 1, 1).Subtract(new DateTime(2026, 1, 2)) + " | " + (a + TimeSpan.FromHours(14)).ToString("s") + " | " + (a - TimeSpan.FromHours(10)).ToString("s"));

        // The comparison.
        DateTime b = new DateTime(2026, 6, 1);
        DateTime c = new DateTime(2026, 6, 1);
        Object boxed = c;
        Console.WriteLine("compare " + (b == c) + " " + (b != c) + " " + (b < a) + " " + (b > a) + " " + (b <= c) + " " + (b >= a) + " " + a.CompareTo(b) + " " + b.CompareTo(a) + " " + b.CompareTo(c) + " " + b.Equals(c) + " " + b.Equals(boxed) + " " + b.Equals("x") + " " + (b.GetHashCode() == c.GetHashCode()));

        // The errors.
        Console.WriteLine("errors " + Try(() => new DateTime(2026, 2, 29)) + " | " + Try(() => new DateTime(2026, 13, 1)) + " | " + Try(() => new DateTime(2026, 0, 1)) + " | " + Try(() => new DateTime(0, 1, 1)) + " | " + Try(() => new DateTime(10000, 1, 1)) + " | " + Try(() => new DateTime(2026, 1, 1, 24, 0, 0)) + " | " + Try(() => new DateTime(2026, 1, 1, 0, 60, 0)) + " | " + Try(() => new DateTime(2026, 1, 1, 0, 0, 0, 1000)));
        Console.WriteLine("limits " + Try(() => new DateTime(-1)) + " | " + Try(() => new DateTime(3155378975999999999 + 1)) + " | " + Try(() => DateTime.MaxValue.AddTicks(1)) + " | " + Try(() => DateTime.MinValue.AddDays(-1)) + " | " + Try(() => DateTime.MaxValue.AddMonths(1)) + " | " + Try(() => DateTime.MinValue.AddYears(-1)) + " | " + Try(() => DateTime.MaxValue.Add(TimeSpan.MaxValue)) + " | " + Try(() => DateTime.MaxValue.AddYears(0)));

        try
        {
            t.ToString("x");
            Console.WriteLine("no format error");
        }
        catch (FormatException)
        {
            Console.WriteLine("format error");
        }

        try
        {
            DateTime.IsLeapYear(0);
            Console.WriteLine("no year error");
        }
        catch (OutOfRangeException)
        {
            Console.WriteLine("year error");
        }

        // The clock of the system.
        DateTime now = DateTime.UtcNow;
        DateTime later = DateTime.UtcNow;
        TimeSpan gap = later - now;
        Console.WriteLine("clock " + (now.Year >= 2025 && now.Year < 2200) + " " + (now > DateTime.UnixEpoch) + " " + (gap < TimeSpan.FromMinutes(1) && gap > TimeSpan.FromMinutes(-1)) + " " + (DateTime.Today.TimeOfDay == TimeSpan.Zero) + " " + (DateTime.Today <= now.AddDays(1)) + " " + (now.Ticks == now.Date.Ticks + now.TimeOfDay.Ticks));

        Console.WriteLine("done");
    }
}
