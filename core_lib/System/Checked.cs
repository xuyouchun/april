/*
 *  System.dll
 *
 *  The arithmetic of checked(expression): it raises OverflowException, when the result does
 *  not fit the type.
 */

////////// ////////// ////////// ////////// //////////

[Internal]
public class Checked
{
    public static Int32 AddInt32(Int32 a, Int32 b)
    {
        Int64 r = (Int64)a + (Int64)b;
        if (r > 2147483647 || r < -2147483647 - 1)
            throw new OverflowException();

        return (Int32)r;
    }

    public static Int32 SubInt32(Int32 a, Int32 b)
    {
        Int64 r = (Int64)a - (Int64)b;
        if (r > 2147483647 || r < -2147483647 - 1)
            throw new OverflowException();

        return (Int32)r;
    }

    public static Int32 MulInt32(Int32 a, Int32 b)
    {
        Int64 r = (Int64)a * (Int64)b;
        if (r > 2147483647 || r < -2147483647 - 1)
            throw new OverflowException();

        return (Int32)r;
    }

    public static UInt32 AddUInt32(UInt32 a, UInt32 b)
    {
        UInt64 r = (UInt64)a + (UInt64)b;
        if (r > 4294967295)
            throw new OverflowException();

        return (UInt32)r;
    }

    public static UInt32 SubUInt32(UInt32 a, UInt32 b)
    {
        if (b > a)
            throw new OverflowException();

        return a - b;
    }

    public static UInt32 MulUInt32(UInt32 a, UInt32 b)
    {
        UInt64 r = (UInt64)a * (UInt64)b;
        if (r > 4294967295)
            throw new OverflowException();

        return (UInt32)r;
    }

    public static Int64 AddInt64(Int64 a, Int64 b)
    {
        Int64 r = a + b;
        if (((a ^ r) & (b ^ r)) < 0)
            throw new OverflowException();

        return r;
    }

    public static Int64 SubInt64(Int64 a, Int64 b)
    {
        Int64 r = a - b;
        if (((a ^ b) & (a ^ r)) < 0)
            throw new OverflowException();

        return r;
    }

    public static Int64 MulInt64(Int64 a, Int64 b)
    {
        if (a == 0 || b == 0)
            return 0;

        Int64 r = a * b;
        if (r / b != a || (a == -1 && b == -9223372036854775807 - 1)
                       || (b == -1 && a == -9223372036854775807 - 1))
            throw new OverflowException();

        return r;
    }

    public static UInt64 AddUInt64(UInt64 a, UInt64 b)
    {
        UInt64 r = a + b;
        if (r < a)
            throw new OverflowException();

        return r;
    }

    public static UInt64 SubUInt64(UInt64 a, UInt64 b)
    {
        if (b > a)
            throw new OverflowException();

        return a - b;
    }

    public static UInt64 MulUInt64(UInt64 a, UInt64 b)
    {
        if (a == 0 || b == 0)
            return (UInt64)0;

        UInt64 r = a * b;
        if (r / b != a)
            throw new OverflowException();

        return r;
    }
}
