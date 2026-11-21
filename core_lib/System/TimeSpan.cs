/*
 *  System.dll
 *
 *  TimeSpan: a length of time.
 */

////////// ////////// ////////// ////////// //////////

// A length of time, positive or negative, in ticks: a tick is 100 nanoseconds. The range is about 29000 years each way
// (Int64). A result that is out of the range is an OverflowException.
public struct TimeSpan : IComparable<TimeSpan>, IEquatable<TimeSpan>, IEquality, IStringable
{
    public const Int64 TicksPerMillisecond = 10000;

    public const Int64 TicksPerSecond = 10000000;

    public const Int64 TicksPerMinute = 600000000;

    public const Int64 TicksPerHour = 36000000000;

    public const Int64 TicksPerDay = 864000000000;

    public TimeSpan(Int64 ticks)
    {
        __ticks = ticks;
    }

    public TimeSpan(Int32 hours, Int32 minutes, Int32 seconds)
    {
        __ticks = checked((Int64)(((Int64)hours * 3600 + (Int64)minutes * 60 + (Int64)seconds) * TicksPerSecond));
    }

    public TimeSpan(Int32 days, Int32 hours, Int32 minutes, Int32 seconds)
    {
        __ticks = checked((Int64)((Int64)days * TicksPerDay + ((Int64)hours * 3600 + (Int64)minutes * 60 + (Int64)seconds) * TicksPerSecond));
    }

    public TimeSpan(Int32 days, Int32 hours, Int32 minutes, Int32 seconds, Int32 milliseconds)
    {
        __ticks = checked((Int64)((Int64)days * TicksPerDay + ((Int64)hours * 3600 + (Int64)minutes * 60 + (Int64)seconds) * TicksPerSecond
                                  + (Int64)milliseconds * TicksPerMillisecond));
    }

    public static TimeSpan Zero => new TimeSpan(0);

    public static TimeSpan MaxValue => new TimeSpan(9223372036854775807);

    public static TimeSpan MinValue => new TimeSpan(-9223372036854775807 - 1);

    // The length in ticks.
    public Int64 Ticks => __ticks;

    // The parts: the days, then the hours (0 to 23), the minutes and the seconds (0 to 59), the milliseconds (0 to 999). They
    // have the sign of the length.
    public Int32 Days => (Int32)(__ticks / TicksPerDay);

    public Int32 Hours => (Int32)((__ticks / TicksPerHour) % 24);

    public Int32 Minutes => (Int32)((__ticks / TicksPerMinute) % 60);

    public Int32 Seconds => (Int32)((__ticks / TicksPerSecond) % 60);

    public Int32 Milliseconds => (Int32)((__ticks / TicksPerMillisecond) % 1000);

    // The whole length in a unit, with a fraction.
    public Double TotalDays => (Double)__ticks / (Double)TicksPerDay;

    public Double TotalHours => (Double)__ticks / (Double)TicksPerHour;

    public Double TotalMinutes => (Double)__ticks / (Double)TicksPerMinute;

    public Double TotalSeconds => (Double)__ticks / (Double)TicksPerSecond;

    public Double TotalMilliseconds => (Double)__ticks / (Double)TicksPerMillisecond;

    // The length of a number of days, hours ... (with a fraction). A number that is not a number (NaN) is an
    // ArgumentException, a number that is too large is an OverflowException. The part of a tick is cut.
    public static TimeSpan FromDays(Double value) => __Interval(value, (Double)TicksPerDay);

    public static TimeSpan FromHours(Double value) => __Interval(value, (Double)TicksPerHour);

    public static TimeSpan FromMinutes(Double value) => __Interval(value, (Double)TicksPerMinute);

    public static TimeSpan FromSeconds(Double value) => __Interval(value, (Double)TicksPerSecond);

    public static TimeSpan FromMilliseconds(Double value) => __Interval(value, (Double)TicksPerMillisecond);

    public static TimeSpan FromTicks(Int64 value) => new TimeSpan(value);

    private static TimeSpan __Interval(Double value, Double scale)
    {
        if (value != value)
            throw new ArgumentException("The value is not a number");

        Double ticks = value * scale;
        if (ticks >= 9223372036854775807.0 || ticks <= -9223372036854775808.0)
            throw new OverflowException();

        return new TimeSpan((Int64)ticks);
    }

    public TimeSpan Add(TimeSpan other) => new TimeSpan(checked((Int64)(__ticks + other.__ticks)));

    public TimeSpan Subtract(TimeSpan other) => new TimeSpan(checked((Int64)(__ticks - other.__ticks)));

    // The same length with the other sign (MinValue has none: an OverflowException).
    public TimeSpan Negate() => new TimeSpan(checked((Int64)(-__ticks)));

    // The length without the sign.
    public TimeSpan Duration() => __ticks < 0 ? Negate() : this;

    public TimeSpan Multiply(Double factor) => __Interval((Double)__ticks, factor);

    public TimeSpan Divide(Double divisor) => __Interval((Double)__ticks, 1.0 / divisor);

    // How many times the other length is in this one.
    public Double Divide(TimeSpan other) => (Double)__ticks / (Double)other.__ticks;

    public Int32 CompareTo(TimeSpan other) => __ticks < other.__ticks ? -1 : (__ticks > other.__ticks ? 1 : 0);

    public Boolean Equals(TimeSpan other) => __ticks == other.__ticks;

    public Boolean Equals(Object? obj) => obj is TimeSpan && __ticks == ((TimeSpan)obj).__ticks;

    public Int32 GetHashCode() => __ticks.GetHashCode();

    // The text of the constant format of .NET: [-][d.]hh:mm:ss[.fffffff], the fraction (7 digits) when there is one.
    public String ToString()
    {
        // The length without the sign, as an unsigned number: MinValue has no negative.
        UInt64 magnitude = __ticks < 0 ? (UInt64)(-(__ticks + 1)) + (UInt64)1 : (UInt64)__ticks;

        UInt64 days = magnitude / (UInt64)TicksPerDay;
        UInt64 rest = magnitude % (UInt64)TicksPerDay;
        Int32 hours = (Int32)(rest / (UInt64)TicksPerHour);
        Int32 minutes = (Int32)((rest / (UInt64)TicksPerMinute) % (UInt64)60);
        Int32 seconds = (Int32)((rest / (UInt64)TicksPerSecond) % (UInt64)60);
        Int32 fraction = (Int32)(rest % (UInt64)TicksPerSecond);

        String s = __ticks < 0 ? "-" : "";
        if (days > (UInt64)0)
            s += days.ToString() + ".";

        s += hours.ToString("D2") + ":" + minutes.ToString("D2") + ":" + seconds.ToString("D2");
        if (fraction != 0)
            s += "." + fraction.ToString("D7");

        return s;
    }

    public static TimeSpan operator +(TimeSpan left, TimeSpan right) => left.Add(right);

    public static TimeSpan operator -(TimeSpan left, TimeSpan right) => left.Subtract(right);

    public static TimeSpan operator -(TimeSpan value) => value.Negate();

    public static TimeSpan operator +(TimeSpan value) => value;

    public static TimeSpan operator *(TimeSpan left, Double right) => left.Multiply(right);

    public static TimeSpan operator *(Double left, TimeSpan right) => right.Multiply(left);

    public static TimeSpan operator /(TimeSpan left, Double right) => left.Divide(right);

    public static Double operator /(TimeSpan left, TimeSpan right) => left.Divide(right);

    public static Boolean operator ==(TimeSpan left, TimeSpan right) => left.__ticks == right.__ticks;

    public static Boolean operator !=(TimeSpan left, TimeSpan right) => left.__ticks != right.__ticks;

    public static Boolean operator <(TimeSpan left, TimeSpan right) => left.__ticks < right.__ticks;

    public static Boolean operator >(TimeSpan left, TimeSpan right) => left.__ticks > right.__ticks;

    public static Boolean operator <=(TimeSpan left, TimeSpan right) => left.__ticks <= right.__ticks;

    public static Boolean operator >=(TimeSpan left, TimeSpan right) => left.__ticks >= right.__ticks;

    private readonly Int64 __ticks;
}
