// The value of a const field that is converted: (Double)Big is 864000000000, not the bits of it; a const of a number that has more
// than 32 bits is the same in the library (System.TimeSpan) as in the program; a const that is given a number of another type is
// of the type of the field.

/* EXPECT

double 864000000000 1000 -7 -864000000000 1728000000000 2000
other 1000 1000 -7 -7 864000000000 1.8446744073709552E+19
kept 864000000000 -864000000000 18446744073709551615 864000000001 864000000001 864000000 4
field type 3 6 5 6 1.5 0.5 0.5
others x True 120
library 864000000000 864000000000 10000000 60 10000
limits 9223372036854775807 -9223372036854775808 9.223372036854776E+18 18446744073709551615 -2147483648

*/

import System;

using System;

class Constants
{
    public const Int64 Big = 864000000000;
    public const Int64 Small = 1000;
    public const Int32 Int = -7;
    public const UInt64 Huge = 18446744073709551615;
    public const Int64 Negative = -864000000000;
    public const Double Half = 0.5;
    public const Double FromInt = 3;
    public const Single Float = 1.5f;
    public const Int64 Widened = 5;
    public const Char Letter = 'x';
    public const Boolean Flag = true;
}

class Program
{
    static Double Twice(Double v) { return v * 2.0; }

    static Single Third(Single v) { return v / 3.0f; }

    static Int64 Plus(Int64 v) { return v + 1; }

    [EntryPoint]
    public static void Main()
    {
        // In the program.
        Console.WriteLine("double " + (Double)Constants.Big + " " + (Double)Constants.Small + " " + (Double)Constants.Int + " " + (Double)Constants.Negative + " " + Twice(Constants.Big) + " " + Twice((Double)Constants.Small));
        Console.WriteLine("other " + (Single)Constants.Small + " " + (Int32)Constants.Small + " " + (Int64)Constants.Int + " " + (Int16)Constants.Int + " " + (UInt64)Constants.Big + " " + (Double)Constants.Huge);
        Console.WriteLine("kept " + Constants.Big + " " + Constants.Negative + " " + Constants.Huge + " " + Plus(Constants.Big) + " " + (Constants.Big + 1) + " " + Constants.Big / 1000 + " " + Constants.Big % 7);
        Console.WriteLine("field type " + Constants.FromInt + " " + Twice(Constants.FromInt) + " " + Constants.Widened + " " + (Constants.Widened + 1) + " " + Constants.Float + " " + Third(Constants.Float) + " " + Constants.Half);
        Console.WriteLine("others " + Constants.Letter + " " + Constants.Flag + " " + (Int32)Constants.Letter);

        // Of the library: a const of the library that is read from its assembly.
        Console.WriteLine("library " + TimeSpan.TicksPerDay + " " + (Double)TimeSpan.TicksPerDay + " " + (Double)TimeSpan.TicksPerSecond + " " + TimeSpan.TicksPerHour / TimeSpan.TicksPerMinute + " " + (Single)TimeSpan.TicksPerMillisecond);
        Console.WriteLine("limits " + Int64.MaxValue + " " + Int64.MinValue + " " + (Double)Int64.MaxValue + " " + UInt64.MaxValue + " " + Int32.MinValue);
    }
}
