/*
 *  System.dll
 *
 *  DateTime, DayOfWeek: a point of time.
 */

////////// ////////// ////////// ////////// //////////

// The days of the week, with the numbers of .NET.
public enum DayOfWeek : Int32
{
    Sunday = 0,
    Monday = 1,
    Tuesday = 2,
    Wednesday = 3,
    Thursday = 4,
    Friday = 5,
    Saturday = 6;
}

////////// ////////// ////////// ////////// //////////

// A point of time, from 0001-01-01 00:00:00 to 9999-12-31 23:59:59.9999999, in the Gregorian calendar, in ticks of 100
// nanoseconds since the beginning of the year 1. All the times are UTC: the library has no time zones and no local time,
// so there is no DateTime.Now, no Kind and no ToLocalTime. The text of a time is the one of the invariant culture.
public struct DateTime : IComparable<DateTime>, IEquatable<DateTime>, IEquality, IStringable
{
    private const Int64 MaxTicks = 3155378975999999999;

    // The days from 0001-01-01 to 1970-01-01.
    private const Int64 DaysTo1970 = 719162;

    public DateTime(Int64 ticks)
    {
        if (ticks < 0 || ticks > MaxTicks)
            throw new OutOfRangeException();

        __ticks = ticks;
    }

    public DateTime(Int32 year, Int32 month, Int32 day)
    {
        __ticks = __TicksOf(year, month, day) ;
    }

    public DateTime(Int32 year, Int32 month, Int32 day, Int32 hour, Int32 minute, Int32 second)
    {
        __ticks = __TicksOf(year, month, day) + __TimeTicks(hour, minute, second, 0);
    }

    public DateTime(Int32 year, Int32 month, Int32 day, Int32 hour, Int32 minute, Int32 second, Int32 millisecond)
    {
        __ticks = __TicksOf(year, month, day) + __TimeTicks(hour, minute, second, millisecond);
    }

    public static DateTime MinValue => new DateTime(0);

    public static DateTime MaxValue => new DateTime(MaxTicks);

    // 1970-01-01 00:00:00.
    public static DateTime UnixEpoch => new DateTime(DaysTo1970 * TimeSpan.TicksPerDay);

    // The time of the clock of the system, UTC. The clock can be set, so a time can be before one that was read earlier: Stopwatch is
    // the clock for measuring.
    public static DateTime UtcNow => new DateTime(Internal.Time_UtcNowTicks());

    // The date of the clock of the system, UTC, at 00:00:00.
    public static DateTime Today => UtcNow.Date;

    private static Int64 __TimeTicks(Int32 hour, Int32 minute, Int32 second, Int32 millisecond)
    {
        if (hour < 0 || hour > 23 || minute < 0 || minute > 59 || second < 0 || second > 59 || millisecond < 0 || millisecond > 999)
            throw new OutOfRangeException();

        return ((Int64)hour * 3600 + (Int64)minute * 60 + (Int64)second) * TimeSpan.TicksPerSecond
             + (Int64)millisecond * TimeSpan.TicksPerMillisecond;
    }

    ////////// ////////// ////////// ////////// //////////
    // The calendar.

    public static Boolean IsLeapYear(Int32 year)
    {
        if (year < 1 || year > 9999)
            throw new OutOfRangeException();

        return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
    }

    public static Int32 DaysInMonth(Int32 year, Int32 month)
    {
        if (month < 1 || month > 12)
            throw new OutOfRangeException();

        if (month == 2)
            return IsLeapYear(year) ? 29 : 28;

        return month == 4 || month == 6 || month == 9 || month == 11 ? 30 : 31;
    }

    // The days from 1970-01-01 to the date (negative before it). The month must be 1 to 12.
    private static Int64 __DaysFromCivil(Int64 year, Int64 month, Int64 day)
    {
        // The year starts in March, so that the day that is added is the last of the year.
        Int64 y = month <= 2 ? year - 1 : year;
        Int64 era = (y >= 0 ? y : y - 399) / 400;
        Int64 yearOfEra = y - era * 400;
        Int64 dayOfYear = (153 * (month + (month > 2 ? -3 : 9)) + 2) / 5 + day - 1;
        Int64 dayOfEra = yearOfEra * 365 + yearOfEra / 4 - yearOfEra / 100 + dayOfYear;
        return era * 146097 + dayOfEra - 719468;
    }

    private static Int64 __TicksOf(Int32 year, Int32 month, Int32 day)
    {
        if (year < 1 || year > 9999 || month < 1 || month > 12 || day < 1 || day > DaysInMonth(year, month))
            throw new OutOfRangeException();

        return (__DaysFromCivil(year, month, day) + DaysTo1970) * TimeSpan.TicksPerDay;
    }

    // The part of the date: 0 the year, 1 the month, 2 the day, 3 the day of the year.
    private Int32 __DatePart(Int32 part)
    {
        Int64 z = __ticks / TimeSpan.TicksPerDay - DaysTo1970 + 719468;
        Int64 era = (z >= 0 ? z : z - 146096) / 146097;
        Int64 dayOfEra = z - era * 146097;
        Int64 yearOfEra = (dayOfEra - dayOfEra / 1460 + dayOfEra / 36524 - dayOfEra / 146096) / 365;
        Int64 dayOfYear = dayOfEra - (365 * yearOfEra + yearOfEra / 4 - yearOfEra / 100);
        Int64 mp = (5 * dayOfYear + 2) / 153;
        Int64 day = dayOfYear - (153 * mp + 2) / 5 + 1;
        Int64 month = mp < 10 ? mp + 3 : mp - 9;
        Int64 year = yearOfEra + era * 400 + (month <= 2 ? 1 : 0);

        if (part == 0)
            return (Int32)year;

        if (part == 1)
            return (Int32)month;

        if (part == 2)
            return (Int32)day;

        // The day of the year: the days of the months before, and the day.
        Int32 total = (Int32)day;
        for (Int32 m = 1; m < (Int32)month; m++)
            total += DaysInMonth((Int32)year, m);

        return total;
    }

    ////////// ////////// ////////// ////////// //////////
    // The parts.

    public Int64 Ticks => __ticks;

    public Int32 Year => __DatePart(0);

    public Int32 Month => __DatePart(1);

    public Int32 Day => __DatePart(2);

    public Int32 DayOfYear => __DatePart(3);

    public Int32 Hour => (Int32)((__ticks / TimeSpan.TicksPerHour) % 24);

    public Int32 Minute => (Int32)((__ticks / TimeSpan.TicksPerMinute) % 60);

    public Int32 Second => (Int32)((__ticks / TimeSpan.TicksPerSecond) % 60);

    public Int32 Millisecond => (Int32)((__ticks / TimeSpan.TicksPerMillisecond) % 1000);

    // 0001-01-01 is a Monday.
    public DayOfWeek DayOfWeek => (DayOfWeek)(Int32)((__ticks / TimeSpan.TicksPerDay + 1) % 7);

    // The date, at 00:00:00.
    public DateTime Date => new DateTime(__ticks - __ticks % TimeSpan.TicksPerDay);

    // The time of the day.
    public TimeSpan TimeOfDay => new TimeSpan(__ticks % TimeSpan.TicksPerDay);

    ////////// ////////// ////////// ////////// //////////
    // The arithmetic. A result out of the range is an OutOfRangeException.

    public DateTime Add(TimeSpan value) => new DateTime(checked((Int64)(__ticks + value.Ticks)));

    public DateTime Subtract(TimeSpan value) => new DateTime(checked((Int64)(__ticks - value.Ticks)));

    public TimeSpan Subtract(DateTime value) => new TimeSpan(__ticks - value.__ticks);

    public DateTime AddTicks(Int64 value) => new DateTime(checked((Int64)(__ticks + value)));

    public DateTime AddDays(Double value) => Add(TimeSpan.FromDays(value));

    public DateTime AddHours(Double value) => Add(TimeSpan.FromHours(value));

    public DateTime AddMinutes(Double value) => Add(TimeSpan.FromMinutes(value));

    public DateTime AddSeconds(Double value) => Add(TimeSpan.FromSeconds(value));

    public DateTime AddMilliseconds(Double value) => Add(TimeSpan.FromMilliseconds(value));

    // A number of months later: the day is cut to the last day of the month when it is not there (01-31 and a month is 02-28).
    public DateTime AddMonths(Int32 months)
    {
        Int32 year = Year;
        Int32 month = Month;
        Int32 day = Day;

        Int64 total = (Int64)year * 12 + (Int64)(month - 1) + (Int64)months;
        if (total < 12 || total > (Int64)9999 * 12 + 11)
            throw new OutOfRangeException();

        Int32 newYear = (Int32)(total / 12);
        Int32 newMonth = (Int32)(total % 12) + 1;
        Int32 limit = DaysInMonth(newYear, newMonth);

        return new DateTime(__TicksOf(newYear, newMonth, day > limit ? limit : day) + __ticks % TimeSpan.TicksPerDay);
    }

    public DateTime AddYears(Int32 years) => AddMonths(checked((Int32)(years * 12)));

    ////////// ////////// ////////// ////////// //////////

    public Int32 CompareTo(DateTime other) => __ticks < other.__ticks ? -1 : (__ticks > other.__ticks ? 1 : 0);

    public Boolean Equals(DateTime other) => __ticks == other.__ticks;

    public Boolean Equals(Object? obj) => obj is DateTime && __ticks == ((DateTime)obj).__ticks;

    public Int32 GetHashCode() => __ticks.GetHashCode();

    // MM/dd/yyyy HH:mm:ss (the general format of the invariant culture).
    public String ToString() => ToString("G");

    // The formats: G (MM/dd/yyyy HH:mm:ss), d (MM/dd/yyyy), T (HH:mm:ss), s (yyyy-MM-ddTHH:mm:ss) and O (yyyy-MM-ddTHH:mm:ss.fffffffZ: the
    // time is UTC).
    public String ToString(String? format)
    {
        String f = format == null || format.Length == 0 ? "G" : format;

        String date = __Pad(Month, 2) + "/" + __Pad(Day, 2) + "/" + __Pad(Year, 4);
        String isoDate = __Pad(Year, 4) + "-" + __Pad(Month, 2) + "-" + __Pad(Day, 2);
        String time = __Pad(Hour, 2) + ":" + __Pad(Minute, 2) + ":" + __Pad(Second, 2);

        if (f == "G")
            return date + " " + time;

        if (f == "d")
            return date;

        if (f == "T")
            return time;

        if (f == "s")
            return isoDate + "T" + time;

        if (f == "O")
            return isoDate + "T" + time + "." + __Pad((Int32)(__ticks % TimeSpan.TicksPerSecond), 7) + "Z";

        throw new FormatException();
    }

    // The number with zeros before it, up to the width.
    private static String __Pad(Int32 value, Int32 width)
    {
        String s = value.ToString();
        while (s.Length < width)
            s = "0" + s;

        return s;
    }

    public static DateTime operator +(DateTime left, TimeSpan right) => left.Add(right);

    public static DateTime operator -(DateTime left, TimeSpan right) => left.Subtract(right);

    public static TimeSpan operator -(DateTime left, DateTime right) => left.Subtract(right);

    public static Boolean operator ==(DateTime left, DateTime right) => left.__ticks == right.__ticks;

    public static Boolean operator !=(DateTime left, DateTime right) => left.__ticks != right.__ticks;

    public static Boolean operator <(DateTime left, DateTime right) => left.__ticks < right.__ticks;

    public static Boolean operator >(DateTime left, DateTime right) => left.__ticks > right.__ticks;

    public static Boolean operator <=(DateTime left, DateTime right) => left.__ticks <= right.__ticks;

    public static Boolean operator >=(DateTime left, DateTime right) => left.__ticks >= right.__ticks;

    private readonly Int64 __ticks;
}
