
/*
 *  System.dll
 *
 *  Int32: the members of the type (the declaration is in +types.cs).
 */

////////// ////////// ////////// ////////// //////////

public partial struct Int32
{
    public Int32 CompareTo(Int32 other) => Primitives.Compare(this, other);

    public Boolean Equals(Int32 other) => Primitives.Same(this, other);

    public Boolean Equals(Object? obj) => obj is Int32 && Primitives.Same(this, (Int32)obj);

    public Int32 GetHashCode() => Primitives.Hash(this);

    public String ToString() => Internal.Int64_ToString(this);

    public String ToString(String? format) => NumberFormat.FormatSigned((Int64)this, 32, format, Globalization.NumberFormatInfo.CurrentInfo);

    public String ToString(String? format, IFormatProvider? provider)
        => NumberFormat.FormatSigned((Int64)this, 32, format, Globalization.NumberFormatInfo.GetInstance(provider));

    // The number of a text: spaces, a sign and digits (see NumberParse). FormatException, OverflowException.
    public static Int32 Parse(String text) => Parse(text, null);

    public static Int32 Parse(String text, IFormatProvider? provider)
    {
        Int64 value;
        Int32 status = NumberParse.Signed(text, Globalization.NumberFormatInfo.GetInstance(provider), -2147483648, 2147483647, out value);
        if (status != 0)
            NumberParse.Throw(status, text, "Int32");

        return (Int32)value;
    }

    public static Boolean TryParse(String? text, out Int32 result) => TryParse(text, null, out result);

    public static Boolean TryParse(String? text, IFormatProvider? provider, out Int32 result)
    {
        Int64 value;
        Int32 status = NumberParse.Signed(text, Globalization.NumberFormatInfo.GetInstance(provider), -2147483648, 2147483647, out value);

        result = (Int32)value;
        return status == 0;
    }

    // ---- generic math: made by tools/gen_numerics.py, do not edit it here ----

    public static Int32 operator +(Int32 left, Int32 right) => (Int32)(left + right);

    public static Int32 operator -(Int32 left, Int32 right) => (Int32)(left - right);

    public static Int32 operator *(Int32 left, Int32 right) => (Int32)(left * right);

    public static Int32 operator /(Int32 left, Int32 right) => (Int32)(left / right);

    public static Int32 operator %(Int32 left, Int32 right) => (Int32)(left % right);

    public static Int32 operator +(Int32 value) => value;

    public static Int32 operator -(Int32 value) => (Int32)(-value);

    public static Int32 operator ++(Int32 value) => (Int32)(value + (Int32)1);

    public static Int32 operator --(Int32 value) => (Int32)(value - (Int32)1);

    public static Int32 operator &(Int32 left, Int32 right) => (Int32)(left & right);

    public static Int32 operator |(Int32 left, Int32 right) => (Int32)(left | right);

    public static Int32 operator ^(Int32 left, Int32 right) => (Int32)(left ^ right);

    public static Int32 operator ~(Int32 value) => (Int32)(~value);

    public static Int32 operator <<(Int32 value, Int32 shiftAmount) => (Int32)(value << (shiftAmount & 31));

    public static Int32 operator >>(Int32 value, Int32 shiftAmount) => (Int32)(value >> (shiftAmount & 31));

    public static Boolean operator ==(Int32 left, Int32 right) => left == right;

    public static Boolean operator !=(Int32 left, Int32 right) => left != right;

    public static Boolean operator <(Int32 left, Int32 right) => left < right;

    public static Boolean operator >(Int32 left, Int32 right) => left > right;

    public static Boolean operator <=(Int32 left, Int32 right) => left <= right;

    public static Boolean operator >=(Int32 left, Int32 right) => left >= right;

    public static Int32 operator checked +(Int32 left, Int32 right) => checked((Int32)(left + right));

    public static Int32 operator checked -(Int32 left, Int32 right) => checked((Int32)(left - right));

    public static Int32 operator checked *(Int32 left, Int32 right) => checked((Int32)(left * right));

    public static Int32 operator checked /(Int32 left, Int32 right) => checked((Int32)(left / right));

    public static Int32 operator checked -(Int32 value) => checked((Int32)(-value));

    public static Int32 operator checked ++(Int32 value) => checked((Int32)(value + (Int32)1));

    public static Int32 operator checked --(Int32 value) => checked((Int32)(value - (Int32)1));

    public static Int32 MinValue => -2147483647 - 1;

    public static Int32 MaxValue => 2147483647;

    public static Int32 Zero => (Int32)0;

    public static Int32 One => (Int32)1;

    public static Int32 Radix => 2;

    public static Int32 AdditiveIdentity => (Int32)0;

    public static Int32 MultiplicativeIdentity => (Int32)1;

    public static Int32 AllBitsSet => (Int32)(-1);

    public static Int32 NegativeOne => (Int32)(-1);

    public static Int32 Abs(Int32 value)
    {
        if (value < (Int32)0)
        {
            Int32 result = (Int32)(-value);
            if (result < (Int32)0)
                throw new OverflowException();

            return result;
        }

        return value;
    }

    public static Boolean IsCanonical(Int32 value) => true;

    public static Boolean IsComplexNumber(Int32 value) => false;

    public static Boolean IsEvenInteger(Int32 value) => (value & (Int32)1) == (Int32)0;

    public static Boolean IsFinite(Int32 value) => true;

    public static Boolean IsImaginaryNumber(Int32 value) => false;

    public static Boolean IsInfinity(Int32 value) => false;

    public static Boolean IsInteger(Int32 value) => true;

    public static Boolean IsNaN(Int32 value) => false;

    public static Boolean IsNegative(Int32 value) => value < (Int32)0;

    public static Boolean IsNegativeInfinity(Int32 value) => false;

    public static Boolean IsNormal(Int32 value) => value != (Int32)0;

    public static Boolean IsOddInteger(Int32 value) => (value & (Int32)1) != (Int32)0;

    public static Boolean IsPositive(Int32 value) => value >= (Int32)0;

    public static Boolean IsPositiveInfinity(Int32 value) => false;

    public static Boolean IsRealNumber(Int32 value) => true;

    public static Boolean IsSubnormal(Int32 value) => false;

    public static Boolean IsZero(Int32 value) => value == (Int32)0;

    public static Int32 MaxMagnitude(Int32 x, Int32 y)
    {
        Int32 ax = x, ay = y;

        if (x < (Int32)0)
        {
            ax = (Int32)(-x);
            if (ax < (Int32)0)
                return x;
        }

        if (y < (Int32)0)
        {
            ay = (Int32)(-y);
            if (ay < (Int32)0)
                return y;
        }

        if (ax > ay)
            return x;

        if (ax == ay)
            return x < (Int32)0? y : x;

        return y;
    }

    public static Int32 MaxMagnitudeNumber(Int32 x, Int32 y) => MaxMagnitude(x, y);

    public static Int32 MinMagnitude(Int32 x, Int32 y)
    {
        Int32 ax = x, ay = y;

        if (x < (Int32)0)
        {
            ax = (Int32)(-x);
            if (ax < (Int32)0)
                return y;
        }

        if (y < (Int32)0)
        {
            ay = (Int32)(-y);
            if (ay < (Int32)0)
                return x;
        }

        if (ax < ay)
            return x;

        if (ax == ay)
            return x < (Int32)0? x : y;

        return y;
    }

    public static Int32 MinMagnitudeNumber(Int32 x, Int32 y) => MinMagnitude(x, y);

    public static Int32 Sign(Int32 value) => value < (Int32)0? -1 : (value > (Int32)0? 1 : 0);

    public static Boolean IsPow2(Int32 value) => value > (Int32)0 && value != (Int32)0 && (value & (value - (Int32)1)) == (Int32)0;

    public static Int32 Log2(Int32 value)
    {
        if (value < (Int32)0)
            throw new OverflowException();

        return (Int32)(32 - 1 - (Int32)LeadingZeroCount((Int32)(value | (Int32)1)));
    }

    public static Tuple<Int32, Int32> DivRem(Int32 left, Int32 right) =>
        new Tuple<Int32, Int32>((Int32)(left / right), (Int32)(left % right));

    public static Int32 LeadingZeroCount(Int32 value)
    {
        UInt32 bits = (UInt32)value;
        Int32 count = 0;

        for (Int32 bit = 32 - 1; bit >= 0; bit--)
        {
            if ((UInt32)((UInt32)(bits >> bit) & (UInt32)1) != (UInt32)0)
                break;

            count++;
        }

        return (Int32)count;
    }

    public static Int32 PopCount(Int32 value)
    {
        UInt32 bits = (UInt32)value;
        Int32 count = 0;

        while (bits != (UInt32)0)
        {
            bits = (UInt32)(bits & (UInt32)(bits - (UInt32)1));
            count++;
        }

        return (Int32)count;
    }

    public static Int32 TrailingZeroCount(Int32 value)
    {
        UInt32 bits = (UInt32)value;
        if (bits == (UInt32)0)
            return (Int32)32;

        Int32 count = 0;
        while ((UInt32)((UInt32)(bits >> count) & (UInt32)1) == (UInt32)0)
            count++;

        return (Int32)count;
    }

    public static Int32 RotateLeft(Int32 value, Int32 rotateAmount)
    {
        Int32 n = rotateAmount & 31;
        if (n == 0)
            return value;

        UInt32 bits = (UInt32)value;
        return (Int32)((UInt32)(bits << n) | (UInt32)(bits >> (32 - n)));
    }

    public static Int32 RotateRight(Int32 value, Int32 rotateAmount)
    {
        Int32 n = rotateAmount & 31;
        if (n == 0)
            return value;

        UInt32 bits = (UInt32)value;
        return (Int32)((UInt32)(bits >> n) | (UInt32)(bits << (32 - n)));
    }

    public Int32 GetByteCount() => 4;

    public Int32 GetShortestBitLength()
    {
        Int32 value = this;

        if (value >= (Int32)0)
            return 32 - (Int32)LeadingZeroCount(value);

        return 33 - (Int32)LeadingZeroCount((Int32)(~value));
    }

    public static Boolean TryConvertFromChecked<TOther>(TOther value, out Int32 result)
        where TOther : Numerics.INumberBase<TOther>
    {
        Boolean negative;
        UInt64 magnitude;

        if (!Numerics.IntegerConversions.TryRead(value, out negative, out magnitude))
        {
            result = (Int32)0;
            return false;
        }

        if (!Numerics.IntegerConversions.Fits(negative, magnitude, 32, true))
            throw new OverflowException();

        UInt64 bits = Numerics.IntegerConversions.Truncate(negative, magnitude);

        result = (Int32)(Int64)bits;
        return true;
    }

    public static Boolean TryConvertToChecked<TTarget>(Int32 value, out TTarget result)
        where TTarget : Numerics.INumberBase<TTarget> => TTarget.TryConvertFromChecked<Int32>(value, out result);

    public static Boolean TryConvertFromSaturating<TOther>(TOther value, out Int32 result)
        where TOther : Numerics.INumberBase<TOther>
    {
        Boolean negative;
        UInt64 magnitude;

        if (!Numerics.IntegerConversions.TryRead(value, out negative, out magnitude))
        {
            result = (Int32)0;
            return false;
        }

        UInt64 bits = Numerics.IntegerConversions.Saturate(negative, magnitude, 32, true);

        result = (Int32)(Int64)bits;
        return true;
    }

    public static Boolean TryConvertToSaturating<TTarget>(Int32 value, out TTarget result)
        where TTarget : Numerics.INumberBase<TTarget> => TTarget.TryConvertFromSaturating<Int32>(value, out result);

    public static Boolean TryConvertFromTruncating<TOther>(TOther value, out Int32 result)
        where TOther : Numerics.INumberBase<TOther>
    {
        Boolean negative;
        UInt64 magnitude;

        if (!Numerics.IntegerConversions.TryRead(value, out negative, out magnitude))
        {
            result = (Int32)0;
            return false;
        }

        UInt64 bits = Numerics.IntegerConversions.Truncate(negative, magnitude);

        result = (Int32)(Int64)bits;
        return true;
    }

    public static Boolean TryConvertToTruncating<TTarget>(Int32 value, out TTarget result)
        where TTarget : Numerics.INumberBase<TTarget> => TTarget.TryConvertFromTruncating<Int32>(value, out result);

    // ---- end of generic math ----
}
