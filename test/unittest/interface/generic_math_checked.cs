// The operators of the integer types in a checked context, through the interfaces of the numbers (IBinaryInteger<T>):
// checked(a + b), checked(-a) and a checked block for ++ and --, raise OverflowException when the result does not fit
// the type. The expected text is made by tools/gen_numerics_test.py (a model of the integers).

/* EXPECT

Int8 a=127 b=3 add=ovf sub=124 mul=ovf div=42 neg=-127 inc=ovf dec=126 wrap=-126
Int8 a=-128 b=5 add=-123 sub=ovf mul=ovf div=-25 neg=ovf inc=-127 dec=ovf wrap=-123
Int8 a=-37 b=11 add=-26 sub=-48 mul=ovf div=-3 neg=37 inc=-36 dec=-38 wrap=-26
Int8 a=37 b=-11 add=26 sub=48 mul=ovf div=-3 neg=-37 inc=38 dec=36 wrap=26
Int8 a=-128 b=-1 add=ovf sub=-127 mul=ovf div=ovf neg=ovf inc=-127 dec=ovf wrap=127
Int8 a=127 b=127 add=ovf sub=0 mul=ovf div=1 neg=-127 inc=ovf dec=126 wrap=-2
Int8 a=-127 b=-1 add=-128 sub=-126 mul=127 div=127 neg=127 inc=-126 dec=-128 wrap=-128
Int8 a=0 b=7 add=7 sub=-7 mul=0 div=0 neg=0 inc=1 dec=-1 wrap=7
Int8 a=127 b=1 add=ovf sub=126 mul=127 div=127 neg=-127 inc=ovf dec=126 wrap=-128
Int8 a=-128 b=1 add=-127 sub=ovf mul=-128 div=-128 neg=ovf inc=-127 dec=ovf wrap=-127
UInt8 a=255 b=3 add=ovf sub=252 mul=ovf div=85 neg=ovf inc=ovf dec=254 wrap=2
UInt8 a=127 b=5 add=132 sub=122 mul=ovf div=25 neg=ovf inc=128 dec=126 wrap=132
UInt8 a=37 b=11 add=48 sub=26 mul=ovf div=3 neg=ovf inc=38 dec=36 wrap=48
UInt8 a=11 b=37 add=48 sub=ovf mul=ovf div=0 neg=ovf inc=12 dec=10 wrap=48
UInt8 a=1 b=255 add=ovf sub=ovf mul=255 div=0 neg=ovf inc=2 dec=0 wrap=0
UInt8 a=255 b=1 add=ovf sub=254 mul=255 div=255 neg=ovf inc=ovf dec=254 wrap=0
UInt8 a=0 b=7 add=7 sub=ovf mul=0 div=0 neg=0 inc=1 dec=ovf wrap=7
UInt8 a=255 b=255 add=ovf sub=0 mul=ovf div=1 neg=ovf inc=ovf dec=254 wrap=254
Int16 a=32767 b=3 add=ovf sub=32764 mul=ovf div=10922 neg=-32767 inc=ovf dec=32766 wrap=-32766
Int16 a=-32768 b=5 add=-32763 sub=ovf mul=ovf div=-6553 neg=ovf inc=-32767 dec=ovf wrap=-32763
Int16 a=-37 b=11 add=-26 sub=-48 mul=-407 div=-3 neg=37 inc=-36 dec=-38 wrap=-26
Int16 a=37 b=-11 add=26 sub=48 mul=-407 div=-3 neg=-37 inc=38 dec=36 wrap=26
Int16 a=-32768 b=-1 add=ovf sub=-32767 mul=ovf div=ovf neg=ovf inc=-32767 dec=ovf wrap=32767
Int16 a=32767 b=32767 add=ovf sub=0 mul=ovf div=1 neg=-32767 inc=ovf dec=32766 wrap=-2
Int16 a=-32767 b=-1 add=-32768 sub=-32766 mul=32767 div=32767 neg=32767 inc=-32766 dec=-32768 wrap=-32768
Int16 a=0 b=7 add=7 sub=-7 mul=0 div=0 neg=0 inc=1 dec=-1 wrap=7
Int16 a=32767 b=1 add=ovf sub=32766 mul=32767 div=32767 neg=-32767 inc=ovf dec=32766 wrap=-32768
Int16 a=-32768 b=1 add=-32767 sub=ovf mul=-32768 div=-32768 neg=ovf inc=-32767 dec=ovf wrap=-32767
UInt16 a=65535 b=3 add=ovf sub=65532 mul=ovf div=21845 neg=ovf inc=ovf dec=65534 wrap=2
UInt16 a=32767 b=5 add=32772 sub=32762 mul=ovf div=6553 neg=ovf inc=32768 dec=32766 wrap=32772
UInt16 a=37 b=11 add=48 sub=26 mul=407 div=3 neg=ovf inc=38 dec=36 wrap=48
UInt16 a=11 b=37 add=48 sub=ovf mul=407 div=0 neg=ovf inc=12 dec=10 wrap=48
UInt16 a=1 b=65535 add=ovf sub=ovf mul=65535 div=0 neg=ovf inc=2 dec=0 wrap=0
UInt16 a=65535 b=1 add=ovf sub=65534 mul=65535 div=65535 neg=ovf inc=ovf dec=65534 wrap=0
UInt16 a=0 b=7 add=7 sub=ovf mul=0 div=0 neg=0 inc=1 dec=ovf wrap=7
UInt16 a=65535 b=65535 add=ovf sub=0 mul=ovf div=1 neg=ovf inc=ovf dec=65534 wrap=65534
Int32 a=2147483647 b=3 add=ovf sub=2147483644 mul=ovf div=715827882 neg=-2147483647 inc=ovf dec=2147483646 wrap=-2147483646
Int32 a=-2147483648 b=5 add=-2147483643 sub=ovf mul=ovf div=-429496729 neg=ovf inc=-2147483647 dec=ovf wrap=-2147483643
Int32 a=-37 b=11 add=-26 sub=-48 mul=-407 div=-3 neg=37 inc=-36 dec=-38 wrap=-26
Int32 a=37 b=-11 add=26 sub=48 mul=-407 div=-3 neg=-37 inc=38 dec=36 wrap=26
Int32 a=-2147483648 b=-1 add=ovf sub=-2147483647 mul=ovf div=ovf neg=ovf inc=-2147483647 dec=ovf wrap=2147483647
Int32 a=2147483647 b=2147483647 add=ovf sub=0 mul=ovf div=1 neg=-2147483647 inc=ovf dec=2147483646 wrap=-2
Int32 a=-2147483647 b=-1 add=-2147483648 sub=-2147483646 mul=2147483647 div=2147483647 neg=2147483647 inc=-2147483646 dec=-2147483648 wrap=-2147483648
Int32 a=0 b=7 add=7 sub=-7 mul=0 div=0 neg=0 inc=1 dec=-1 wrap=7
Int32 a=2147483647 b=1 add=ovf sub=2147483646 mul=2147483647 div=2147483647 neg=-2147483647 inc=ovf dec=2147483646 wrap=-2147483648
Int32 a=-2147483648 b=1 add=-2147483647 sub=ovf mul=-2147483648 div=-2147483648 neg=ovf inc=-2147483647 dec=ovf wrap=-2147483647
UInt32 a=4294967295 b=3 add=ovf sub=4294967292 mul=ovf div=1431655765 neg=ovf inc=ovf dec=4294967294 wrap=2
UInt32 a=2147483647 b=5 add=2147483652 sub=2147483642 mul=ovf div=429496729 neg=ovf inc=2147483648 dec=2147483646 wrap=2147483652
UInt32 a=37 b=11 add=48 sub=26 mul=407 div=3 neg=ovf inc=38 dec=36 wrap=48
UInt32 a=11 b=37 add=48 sub=ovf mul=407 div=0 neg=ovf inc=12 dec=10 wrap=48
UInt32 a=1 b=4294967295 add=ovf sub=ovf mul=4294967295 div=0 neg=ovf inc=2 dec=0 wrap=0
UInt32 a=4294967295 b=1 add=ovf sub=4294967294 mul=4294967295 div=4294967295 neg=ovf inc=ovf dec=4294967294 wrap=0
UInt32 a=0 b=7 add=7 sub=ovf mul=0 div=0 neg=0 inc=1 dec=ovf wrap=7
UInt32 a=4294967295 b=4294967295 add=ovf sub=0 mul=ovf div=1 neg=ovf inc=ovf dec=4294967294 wrap=4294967294
Int64 a=9223372036854775807 b=3 add=ovf sub=9223372036854775804 mul=ovf div=3074457345618258602 neg=-9223372036854775807 inc=ovf dec=9223372036854775806 wrap=-9223372036854775806
Int64 a=-9223372036854775808 b=5 add=-9223372036854775803 sub=ovf mul=ovf div=-1844674407370955161 neg=ovf inc=-9223372036854775807 dec=ovf wrap=-9223372036854775803
Int64 a=-37 b=11 add=-26 sub=-48 mul=-407 div=-3 neg=37 inc=-36 dec=-38 wrap=-26
Int64 a=37 b=-11 add=26 sub=48 mul=-407 div=-3 neg=-37 inc=38 dec=36 wrap=26
Int64 a=-9223372036854775808 b=-1 add=ovf sub=-9223372036854775807 mul=ovf div=ovf neg=ovf inc=-9223372036854775807 dec=ovf wrap=9223372036854775807
Int64 a=9223372036854775807 b=9223372036854775807 add=ovf sub=0 mul=ovf div=1 neg=-9223372036854775807 inc=ovf dec=9223372036854775806 wrap=-2
Int64 a=-9223372036854775807 b=-1 add=-9223372036854775808 sub=-9223372036854775806 mul=9223372036854775807 div=9223372036854775807 neg=9223372036854775807 inc=-9223372036854775806 dec=-9223372036854775808 wrap=-9223372036854775808
Int64 a=0 b=7 add=7 sub=-7 mul=0 div=0 neg=0 inc=1 dec=-1 wrap=7
Int64 a=9223372036854775807 b=1 add=ovf sub=9223372036854775806 mul=9223372036854775807 div=9223372036854775807 neg=-9223372036854775807 inc=ovf dec=9223372036854775806 wrap=-9223372036854775808
Int64 a=-9223372036854775808 b=1 add=-9223372036854775807 sub=ovf mul=-9223372036854775808 div=-9223372036854775808 neg=ovf inc=-9223372036854775807 dec=ovf wrap=-9223372036854775807
UInt64 a=18446744073709551615 b=3 add=ovf sub=18446744073709551612 mul=ovf div=6148914691236517205 neg=ovf inc=ovf dec=18446744073709551614 wrap=2
UInt64 a=9223372036854775807 b=5 add=9223372036854775812 sub=9223372036854775802 mul=ovf div=1844674407370955161 neg=ovf inc=9223372036854775808 dec=9223372036854775806 wrap=9223372036854775812
UInt64 a=37 b=11 add=48 sub=26 mul=407 div=3 neg=ovf inc=38 dec=36 wrap=48
UInt64 a=11 b=37 add=48 sub=ovf mul=407 div=0 neg=ovf inc=12 dec=10 wrap=48
UInt64 a=1 b=18446744073709551615 add=ovf sub=ovf mul=18446744073709551615 div=0 neg=ovf inc=2 dec=0 wrap=0
UInt64 a=18446744073709551615 b=1 add=ovf sub=18446744073709551614 mul=18446744073709551615 div=18446744073709551615 neg=ovf inc=ovf dec=18446744073709551614 wrap=0
UInt64 a=0 b=7 add=7 sub=ovf mul=0 div=0 neg=0 inc=1 dec=ovf wrap=7
UInt64 a=18446744073709551615 b=18446744073709551615 add=ovf sub=0 mul=ovf div=1 neg=ovf inc=ovf dec=18446744073709551614 wrap=18446744073709551614

*/

import System;

using System;
using System.Numerics;

class Util
{
    public static String Checked<T>(T a, T b) where T : IBinaryInteger<T>, IStringable
    {
        String r = "a=" + a.ToString() + " b=" + b.ToString();

        try { T t = checked(a + b); r += " add=" + t.ToString(); } catch (OverflowException) { r += " add=ovf"; }
        try { T t = checked(a - b); r += " sub=" + t.ToString(); } catch (OverflowException) { r += " sub=ovf"; }
        try { T t = checked(a * b); r += " mul=" + t.ToString(); } catch (OverflowException) { r += " mul=ovf"; }
        try { T t = checked(a / b); r += " div=" + t.ToString(); } catch (OverflowException) { r += " div=ovf"; }
        try { T t = checked(-a); r += " neg=" + t.ToString(); } catch (OverflowException) { r += " neg=ovf"; }

        try
        {
            T t = a;
            checked { t++; }
            r += " inc=" + t.ToString();
        }
        catch (OverflowException)
        {
            r += " inc=ovf";
        }

        try
        {
            T t = a;
            checked { t--; }
            r += " dec=" + t.ToString();
        }
        catch (OverflowException)
        {
            r += " dec=ovf";
        }

        // Not checked: the same operators wrap.
        T w = a + b;
        r += " wrap=" + w.ToString();
        return r;
    }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine("Int8 " + Util.Checked(Int8.MaxValue, (Int8)3));
        Console.WriteLine("Int8 " + Util.Checked(Int8.MinValue, (Int8)5));
        Console.WriteLine("Int8 " + Util.Checked((Int8)(-37), (Int8)11));
        Console.WriteLine("Int8 " + Util.Checked((Int8)37, (Int8)(-11)));
        Console.WriteLine("Int8 " + Util.Checked(Int8.MinValue, (Int8)(-1)));
        Console.WriteLine("Int8 " + Util.Checked(Int8.MaxValue, Int8.MaxValue));
        Console.WriteLine("Int8 " + Util.Checked((Int8)(-127), (Int8)(-1)));
        Console.WriteLine("Int8 " + Util.Checked((Int8)0, (Int8)7));
        Console.WriteLine("Int8 " + Util.Checked(Int8.MaxValue, (Int8)1));
        Console.WriteLine("Int8 " + Util.Checked(Int8.MinValue, (Int8)1));
        Console.WriteLine("UInt8 " + Util.Checked(UInt8.MaxValue, (UInt8)3));
        Console.WriteLine("UInt8 " + Util.Checked((UInt8)127, (UInt8)5));
        Console.WriteLine("UInt8 " + Util.Checked((UInt8)37, (UInt8)11));
        Console.WriteLine("UInt8 " + Util.Checked((UInt8)11, (UInt8)37));
        Console.WriteLine("UInt8 " + Util.Checked((UInt8)1, UInt8.MaxValue));
        Console.WriteLine("UInt8 " + Util.Checked(UInt8.MaxValue, (UInt8)1));
        Console.WriteLine("UInt8 " + Util.Checked(UInt8.MinValue, (UInt8)7));
        Console.WriteLine("UInt8 " + Util.Checked(UInt8.MaxValue, UInt8.MaxValue));
        Console.WriteLine("Int16 " + Util.Checked(Int16.MaxValue, (Int16)3));
        Console.WriteLine("Int16 " + Util.Checked(Int16.MinValue, (Int16)5));
        Console.WriteLine("Int16 " + Util.Checked((Int16)(-37), (Int16)11));
        Console.WriteLine("Int16 " + Util.Checked((Int16)37, (Int16)(-11)));
        Console.WriteLine("Int16 " + Util.Checked(Int16.MinValue, (Int16)(-1)));
        Console.WriteLine("Int16 " + Util.Checked(Int16.MaxValue, Int16.MaxValue));
        Console.WriteLine("Int16 " + Util.Checked((Int16)(-32767), (Int16)(-1)));
        Console.WriteLine("Int16 " + Util.Checked((Int16)0, (Int16)7));
        Console.WriteLine("Int16 " + Util.Checked(Int16.MaxValue, (Int16)1));
        Console.WriteLine("Int16 " + Util.Checked(Int16.MinValue, (Int16)1));
        Console.WriteLine("UInt16 " + Util.Checked(UInt16.MaxValue, (UInt16)3));
        Console.WriteLine("UInt16 " + Util.Checked((UInt16)32767, (UInt16)5));
        Console.WriteLine("UInt16 " + Util.Checked((UInt16)37, (UInt16)11));
        Console.WriteLine("UInt16 " + Util.Checked((UInt16)11, (UInt16)37));
        Console.WriteLine("UInt16 " + Util.Checked((UInt16)1, UInt16.MaxValue));
        Console.WriteLine("UInt16 " + Util.Checked(UInt16.MaxValue, (UInt16)1));
        Console.WriteLine("UInt16 " + Util.Checked(UInt16.MinValue, (UInt16)7));
        Console.WriteLine("UInt16 " + Util.Checked(UInt16.MaxValue, UInt16.MaxValue));
        Console.WriteLine("Int32 " + Util.Checked(Int32.MaxValue, (Int32)3));
        Console.WriteLine("Int32 " + Util.Checked(Int32.MinValue, (Int32)5));
        Console.WriteLine("Int32 " + Util.Checked((Int32)(-37), (Int32)11));
        Console.WriteLine("Int32 " + Util.Checked((Int32)37, (Int32)(-11)));
        Console.WriteLine("Int32 " + Util.Checked(Int32.MinValue, (Int32)(-1)));
        Console.WriteLine("Int32 " + Util.Checked(Int32.MaxValue, Int32.MaxValue));
        Console.WriteLine("Int32 " + Util.Checked((Int32)(-2147483647), (Int32)(-1)));
        Console.WriteLine("Int32 " + Util.Checked((Int32)0, (Int32)7));
        Console.WriteLine("Int32 " + Util.Checked(Int32.MaxValue, (Int32)1));
        Console.WriteLine("Int32 " + Util.Checked(Int32.MinValue, (Int32)1));
        Console.WriteLine("UInt32 " + Util.Checked(UInt32.MaxValue, (UInt32)3));
        Console.WriteLine("UInt32 " + Util.Checked((UInt32)2147483647, (UInt32)5));
        Console.WriteLine("UInt32 " + Util.Checked((UInt32)37, (UInt32)11));
        Console.WriteLine("UInt32 " + Util.Checked((UInt32)11, (UInt32)37));
        Console.WriteLine("UInt32 " + Util.Checked((UInt32)1, UInt32.MaxValue));
        Console.WriteLine("UInt32 " + Util.Checked(UInt32.MaxValue, (UInt32)1));
        Console.WriteLine("UInt32 " + Util.Checked(UInt32.MinValue, (UInt32)7));
        Console.WriteLine("UInt32 " + Util.Checked(UInt32.MaxValue, UInt32.MaxValue));
        Console.WriteLine("Int64 " + Util.Checked(Int64.MaxValue, (Int64)3));
        Console.WriteLine("Int64 " + Util.Checked(Int64.MinValue, (Int64)5));
        Console.WriteLine("Int64 " + Util.Checked((Int64)(-37), (Int64)11));
        Console.WriteLine("Int64 " + Util.Checked((Int64)37, (Int64)(-11)));
        Console.WriteLine("Int64 " + Util.Checked(Int64.MinValue, (Int64)(-1)));
        Console.WriteLine("Int64 " + Util.Checked(Int64.MaxValue, Int64.MaxValue));
        Console.WriteLine("Int64 " + Util.Checked((Int64)(-9223372036854775807), (Int64)(-1)));
        Console.WriteLine("Int64 " + Util.Checked((Int64)0, (Int64)7));
        Console.WriteLine("Int64 " + Util.Checked(Int64.MaxValue, (Int64)1));
        Console.WriteLine("Int64 " + Util.Checked(Int64.MinValue, (Int64)1));
        Console.WriteLine("UInt64 " + Util.Checked(UInt64.MaxValue, (UInt64)3));
        Console.WriteLine("UInt64 " + Util.Checked((UInt64)9223372036854775807, (UInt64)5));
        Console.WriteLine("UInt64 " + Util.Checked((UInt64)37, (UInt64)11));
        Console.WriteLine("UInt64 " + Util.Checked((UInt64)11, (UInt64)37));
        Console.WriteLine("UInt64 " + Util.Checked((UInt64)1, UInt64.MaxValue));
        Console.WriteLine("UInt64 " + Util.Checked(UInt64.MaxValue, (UInt64)1));
        Console.WriteLine("UInt64 " + Util.Checked(UInt64.MinValue, (UInt64)7));
        Console.WriteLine("UInt64 " + Util.Checked(UInt64.MaxValue, UInt64.MaxValue));
    }
}
