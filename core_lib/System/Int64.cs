
/*
 *  System.dll
 *
 *  Int64: the members of the type (the declaration is in +types.cs).
 */

////////// ////////// ////////// ////////// //////////

public partial struct Int64
{
    public Int32 CompareTo(Int64 other) => Primitives.Compare(this, other);

    public Boolean Equals(Int64 other) => Primitives.Same(this, other);

    public Boolean Equals(Object? obj) => obj is Int64 && Primitives.Same(this, (Int64)obj);

    public Int32 GetHashCode() => Primitives.Hash(this);

    public String ToString() => Internal.Int64_ToString(this);

    public String ToString(String? format) => NumberFormat.FormatSigned((Int64)this, 64, format, Globalization.NumberFormatInfo.CurrentInfo);

    public String ToString(String? format, IFormatProvider? provider)
        => NumberFormat.FormatSigned((Int64)this, 64, format, Globalization.NumberFormatInfo.GetInstance(provider));

    // ---- generic math: made by tools/gen_numerics.py, do not edit it here ----

    public static Int64 operator +(Int64 left, Int64 right) => (Int64)(left + right);

    public static Int64 operator -(Int64 left, Int64 right) => (Int64)(left - right);

    public static Int64 operator *(Int64 left, Int64 right) => (Int64)(left * right);

    public static Int64 operator /(Int64 left, Int64 right) => (Int64)(left / right);

    public static Int64 operator %(Int64 left, Int64 right) => (Int64)(left % right);

    public static Int64 operator +(Int64 value) => value;

    public static Int64 operator -(Int64 value) => (Int64)(-value);

    public static Int64 operator ++(Int64 value) => (Int64)(value + (Int64)1);

    public static Int64 operator --(Int64 value) => (Int64)(value - (Int64)1);

    public static Int64 operator &(Int64 left, Int64 right) => (Int64)(left & right);

    public static Int64 operator |(Int64 left, Int64 right) => (Int64)(left | right);

    public static Int64 operator ^(Int64 left, Int64 right) => (Int64)(left ^ right);

    public static Int64 operator ~(Int64 value) => (Int64)(~value);

    public static Int64 operator <<(Int64 value, Int32 shiftAmount) => (Int64)(value << (shiftAmount & 63));

    public static Int64 operator >>(Int64 value, Int32 shiftAmount) => (Int64)(value >> (shiftAmount & 63));

    public static Boolean operator ==(Int64 left, Int64 right) => left == right;

    public static Boolean operator !=(Int64 left, Int64 right) => left != right;

    public static Boolean operator <(Int64 left, Int64 right) => left < right;

    public static Boolean operator >(Int64 left, Int64 right) => left > right;

    public static Boolean operator <=(Int64 left, Int64 right) => left <= right;

    public static Boolean operator >=(Int64 left, Int64 right) => left >= right;

    public static Int64 operator checked +(Int64 left, Int64 right) => checked((Int64)(left + right));

    public static Int64 operator checked -(Int64 left, Int64 right) => checked((Int64)(left - right));

    public static Int64 operator checked *(Int64 left, Int64 right) => checked((Int64)(left * right));

    public static Int64 operator checked /(Int64 left, Int64 right) => checked((Int64)(left / right));

    public static Int64 operator checked -(Int64 value) => checked((Int64)(-value));

    public static Int64 operator checked ++(Int64 value) => checked((Int64)(value + (Int64)1));

    public static Int64 operator checked --(Int64 value) => checked((Int64)(value - (Int64)1));

    public static Int64 MinValue => -9223372036854775807 - 1;

    public static Int64 MaxValue => 9223372036854775807;

    public static Int64 Zero => (Int64)0;

    public static Int64 One => (Int64)1;

    public static Int32 Radix => 2;

    public static Int64 AdditiveIdentity => (Int64)0;

    public static Int64 MultiplicativeIdentity => (Int64)1;

    public static Int64 AllBitsSet => (Int64)(-1);

    public static Int64 NegativeOne => (Int64)(-1);

    public static Int64 Abs(Int64 value)
    {
        if (value < (Int64)0)
        {
            Int64 result = (Int64)(-value);
            if (result < (Int64)0)
                throw new OverflowException();

            return result;
        }

        return value;
    }

    public static Boolean IsCanonical(Int64 value) => true;

    public static Boolean IsComplexNumber(Int64 value) => false;

    public static Boolean IsEvenInteger(Int64 value) => (value & (Int64)1) == (Int64)0;

    public static Boolean IsFinite(Int64 value) => true;

    public static Boolean IsImaginaryNumber(Int64 value) => false;

    public static Boolean IsInfinity(Int64 value) => false;

    public static Boolean IsInteger(Int64 value) => true;

    public static Boolean IsNaN(Int64 value) => false;

    public static Boolean IsNegative(Int64 value) => value < (Int64)0;

    public static Boolean IsNegativeInfinity(Int64 value) => false;

    public static Boolean IsNormal(Int64 value) => value != (Int64)0;

    public static Boolean IsOddInteger(Int64 value) => (value & (Int64)1) != (Int64)0;

    public static Boolean IsPositive(Int64 value) => value >= (Int64)0;

    public static Boolean IsPositiveInfinity(Int64 value) => false;

    public static Boolean IsRealNumber(Int64 value) => true;

    public static Boolean IsSubnormal(Int64 value) => false;

    public static Boolean IsZero(Int64 value) => value == (Int64)0;

    public static Int64 MaxMagnitude(Int64 x, Int64 y)
    {
        Int64 ax = x, ay = y;

        if (x < (Int64)0)
        {
            ax = (Int64)(-x);
            if (ax < (Int64)0)
                return x;
        }

        if (y < (Int64)0)
        {
            ay = (Int64)(-y);
            if (ay < (Int64)0)
                return y;
        }

        if (ax > ay)
            return x;

        if (ax == ay)
            return x < (Int64)0? y : x;

        return y;
    }

    public static Int64 MaxMagnitudeNumber(Int64 x, Int64 y) => MaxMagnitude(x, y);

    public static Int64 MinMagnitude(Int64 x, Int64 y)
    {
        Int64 ax = x, ay = y;

        if (x < (Int64)0)
        {
            ax = (Int64)(-x);
            if (ax < (Int64)0)
                return y;
        }

        if (y < (Int64)0)
        {
            ay = (Int64)(-y);
            if (ay < (Int64)0)
                return x;
        }

        if (ax < ay)
            return x;

        if (ax == ay)
            return x < (Int64)0? x : y;

        return y;
    }

    public static Int64 MinMagnitudeNumber(Int64 x, Int64 y) => MinMagnitude(x, y);

    public static Int32 Sign(Int64 value) => value < (Int64)0? -1 : (value > (Int64)0? 1 : 0);

    public static Boolean IsPow2(Int64 value) => value > (Int64)0 && value != (Int64)0 && (value & (value - (Int64)1)) == (Int64)0;

    public static Int64 Log2(Int64 value)
    {
        if (value < (Int64)0)
            throw new OverflowException();

        return (Int64)(64 - 1 - (Int32)LeadingZeroCount((Int64)(value | (Int64)1)));
    }

    public static Tuple<Int64, Int64> DivRem(Int64 left, Int64 right) =>
        new Tuple<Int64, Int64>((Int64)(left / right), (Int64)(left % right));

    public static Int64 LeadingZeroCount(Int64 value)
    {
        UInt64 bits = (UInt64)value;
        Int32 count = 0;

        for (Int32 bit = 64 - 1; bit >= 0; bit--)
        {
            if ((UInt64)((UInt64)(bits >> bit) & (UInt64)1) != (UInt64)0)
                break;

            count++;
        }

        return (Int64)count;
    }

    public static Int64 PopCount(Int64 value)
    {
        UInt64 bits = (UInt64)value;
        Int32 count = 0;

        while (bits != (UInt64)0)
        {
            bits = (UInt64)(bits & (UInt64)(bits - (UInt64)1));
            count++;
        }

        return (Int64)count;
    }

    public static Int64 TrailingZeroCount(Int64 value)
    {
        UInt64 bits = (UInt64)value;
        if (bits == (UInt64)0)
            return (Int64)64;

        Int32 count = 0;
        while ((UInt64)((UInt64)(bits >> count) & (UInt64)1) == (UInt64)0)
            count++;

        return (Int64)count;
    }

    public static Int64 RotateLeft(Int64 value, Int32 rotateAmount)
    {
        Int32 n = rotateAmount & 63;
        if (n == 0)
            return value;

        UInt64 bits = (UInt64)value;
        return (Int64)((UInt64)(bits << n) | (UInt64)(bits >> (64 - n)));
    }

    public static Int64 RotateRight(Int64 value, Int32 rotateAmount)
    {
        Int32 n = rotateAmount & 63;
        if (n == 0)
            return value;

        UInt64 bits = (UInt64)value;
        return (Int64)((UInt64)(bits >> n) | (UInt64)(bits << (64 - n)));
    }

    public Int32 GetByteCount() => 8;

    public Int32 GetShortestBitLength()
    {
        Int64 value = this;

        if (value >= (Int64)0)
            return 64 - (Int32)LeadingZeroCount(value);

        return 65 - (Int32)LeadingZeroCount((Int64)(~value));
    }

    public static Boolean TryConvertFromChecked<TOther>(TOther value, out Int64 result)
        where TOther : Numerics.INumberBase<TOther>
    {
        Boolean negative;
        UInt64 magnitude;

        if (!Numerics.IntegerConversions.TryRead(value, out negative, out magnitude))
        {
            result = (Int64)0;
            return false;
        }

        if (!Numerics.IntegerConversions.Fits(negative, magnitude, 64, true))
            throw new OverflowException();

        UInt64 bits = Numerics.IntegerConversions.Truncate(negative, magnitude);

        result = (Int64)(Int64)bits;
        return true;
    }

    public static Boolean TryConvertToChecked<TTarget>(Int64 value, out TTarget result)
        where TTarget : Numerics.INumberBase<TTarget> => TTarget.TryConvertFromChecked<Int64>(value, out result);

    public static Boolean TryConvertFromSaturating<TOther>(TOther value, out Int64 result)
        where TOther : Numerics.INumberBase<TOther>
    {
        Boolean negative;
        UInt64 magnitude;

        if (!Numerics.IntegerConversions.TryRead(value, out negative, out magnitude))
        {
            result = (Int64)0;
            return false;
        }

        UInt64 bits = Numerics.IntegerConversions.Saturate(negative, magnitude, 64, true);

        result = (Int64)(Int64)bits;
        return true;
    }

    public static Boolean TryConvertToSaturating<TTarget>(Int64 value, out TTarget result)
        where TTarget : Numerics.INumberBase<TTarget> => TTarget.TryConvertFromSaturating<Int64>(value, out result);

    public static Boolean TryConvertFromTruncating<TOther>(TOther value, out Int64 result)
        where TOther : Numerics.INumberBase<TOther>
    {
        Boolean negative;
        UInt64 magnitude;

        if (!Numerics.IntegerConversions.TryRead(value, out negative, out magnitude))
        {
            result = (Int64)0;
            return false;
        }

        UInt64 bits = Numerics.IntegerConversions.Truncate(negative, magnitude);

        result = (Int64)(Int64)bits;
        return true;
    }

    public static Boolean TryConvertToTruncating<TTarget>(Int64 value, out TTarget result)
        where TTarget : Numerics.INumberBase<TTarget> => TTarget.TryConvertFromTruncating<Int64>(value, out result);

    // ---- end of generic math ----
}
