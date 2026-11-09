
/*
 *  System.dll
 *
 *  UInt64: the members of the type (the declaration is in +types.cs).
 */

////////// ////////// ////////// ////////// //////////

public partial struct UInt64
{
    public Int32 CompareTo(UInt64 other) => Primitives.Compare(this, other);

    public Boolean Equals(UInt64 other) => Primitives.Same(this, other);

    public Boolean Equals(Object? obj) => obj is UInt64 && Primitives.Same(this, (UInt64)obj);

    public Int32 GetHashCode() => Primitives.Hash(this);

    public String ToString() => Internal.UInt64_ToString(this);

    public String ToString(String? format) => NumberFormat.FormatUnsigned((UInt64)this, 64, format, Globalization.NumberFormatInfo.CurrentInfo);

    public String ToString(String? format, IFormatProvider? provider)
        => NumberFormat.FormatUnsigned((UInt64)this, 64, format, Globalization.NumberFormatInfo.GetInstance(provider));

    // ---- generic math: made by tools/gen_numerics.py, do not edit it here ----

    public static UInt64 operator +(UInt64 left, UInt64 right) => (UInt64)(left + right);

    public static UInt64 operator -(UInt64 left, UInt64 right) => (UInt64)(left - right);

    public static UInt64 operator *(UInt64 left, UInt64 right) => (UInt64)(left * right);

    public static UInt64 operator /(UInt64 left, UInt64 right) => (UInt64)(left / right);

    public static UInt64 operator %(UInt64 left, UInt64 right) => (UInt64)(left % right);

    public static UInt64 operator +(UInt64 value) => value;

    public static UInt64 operator -(UInt64 value) => (UInt64)(~value + (UInt64)1);

    public static UInt64 operator ++(UInt64 value) => (UInt64)(value + (UInt64)1);

    public static UInt64 operator --(UInt64 value) => (UInt64)(value - (UInt64)1);

    public static UInt64 operator &(UInt64 left, UInt64 right) => (UInt64)(left & right);

    public static UInt64 operator |(UInt64 left, UInt64 right) => (UInt64)(left | right);

    public static UInt64 operator ^(UInt64 left, UInt64 right) => (UInt64)(left ^ right);

    public static UInt64 operator ~(UInt64 value) => (UInt64)(~value);

    public static UInt64 operator <<(UInt64 value, Int32 shiftAmount) => (UInt64)(value << (shiftAmount & 63));

    public static UInt64 operator >>(UInt64 value, Int32 shiftAmount) => (UInt64)(value >> (shiftAmount & 63));

    public static Boolean operator ==(UInt64 left, UInt64 right) => left == right;

    public static Boolean operator !=(UInt64 left, UInt64 right) => left != right;

    public static Boolean operator <(UInt64 left, UInt64 right) => left < right;

    public static Boolean operator >(UInt64 left, UInt64 right) => left > right;

    public static Boolean operator <=(UInt64 left, UInt64 right) => left <= right;

    public static Boolean operator >=(UInt64 left, UInt64 right) => left >= right;

    public static UInt64 operator checked +(UInt64 left, UInt64 right) => checked((UInt64)(left + right));

    public static UInt64 operator checked -(UInt64 left, UInt64 right) => checked((UInt64)(left - right));

    public static UInt64 operator checked *(UInt64 left, UInt64 right) => checked((UInt64)(left * right));

    public static UInt64 operator checked /(UInt64 left, UInt64 right) => checked((UInt64)(left / right));

    public static UInt64 operator checked -(UInt64 value) => checked((UInt64)((UInt64)0 - value));

    public static UInt64 operator checked ++(UInt64 value) => checked((UInt64)(value + (UInt64)1));

    public static UInt64 operator checked --(UInt64 value) => checked((UInt64)(value - (UInt64)1));

    public static UInt64 MinValue => (UInt64)0;

    public static UInt64 MaxValue => (UInt64)18446744073709551615;

    public static UInt64 Zero => (UInt64)0;

    public static UInt64 One => (UInt64)1;

    public static Int32 Radix => 2;

    public static UInt64 AdditiveIdentity => (UInt64)0;

    public static UInt64 MultiplicativeIdentity => (UInt64)1;

    public static UInt64 AllBitsSet => (UInt64)18446744073709551615;

    public static UInt64 Abs(UInt64 value) => value;

    public static Boolean IsCanonical(UInt64 value) => true;

    public static Boolean IsComplexNumber(UInt64 value) => false;

    public static Boolean IsEvenInteger(UInt64 value) => (value & (UInt64)1) == (UInt64)0;

    public static Boolean IsFinite(UInt64 value) => true;

    public static Boolean IsImaginaryNumber(UInt64 value) => false;

    public static Boolean IsInfinity(UInt64 value) => false;

    public static Boolean IsInteger(UInt64 value) => true;

    public static Boolean IsNaN(UInt64 value) => false;

    public static Boolean IsNegative(UInt64 value) => false;

    public static Boolean IsNegativeInfinity(UInt64 value) => false;

    public static Boolean IsNormal(UInt64 value) => value != (UInt64)0;

    public static Boolean IsOddInteger(UInt64 value) => (value & (UInt64)1) != (UInt64)0;

    public static Boolean IsPositive(UInt64 value) => true;

    public static Boolean IsPositiveInfinity(UInt64 value) => false;

    public static Boolean IsRealNumber(UInt64 value) => true;

    public static Boolean IsSubnormal(UInt64 value) => false;

    public static Boolean IsZero(UInt64 value) => value == (UInt64)0;

    public static UInt64 MaxMagnitude(UInt64 x, UInt64 y) => x > y? x : y;

    public static UInt64 MaxMagnitudeNumber(UInt64 x, UInt64 y) => x > y? x : y;

    public static UInt64 MinMagnitude(UInt64 x, UInt64 y) => x < y? x : y;

    public static UInt64 MinMagnitudeNumber(UInt64 x, UInt64 y) => x < y? x : y;

    public static Int32 Sign(UInt64 value) => value == (UInt64)0? 0 : 1;

    public static Boolean IsPow2(UInt64 value) => value != (UInt64)0 && (value & (value - (UInt64)1)) == (UInt64)0;

    public static UInt64 Log2(UInt64 value) => (UInt64)(64 - 1 - (Int32)LeadingZeroCount((UInt64)(value | (UInt64)1)));

    public static Tuple<UInt64, UInt64> DivRem(UInt64 left, UInt64 right) =>
        new Tuple<UInt64, UInt64>((UInt64)(left / right), (UInt64)(left % right));

    public static UInt64 LeadingZeroCount(UInt64 value)
    {
        UInt64 bits = (UInt64)value;
        Int32 count = 0;

        for (Int32 bit = 64 - 1; bit >= 0; bit--)
        {
            if ((UInt64)((UInt64)(bits >> bit) & (UInt64)1) != (UInt64)0)
                break;

            count++;
        }

        return (UInt64)count;
    }

    public static UInt64 PopCount(UInt64 value)
    {
        UInt64 bits = (UInt64)value;
        Int32 count = 0;

        while (bits != (UInt64)0)
        {
            bits = (UInt64)(bits & (UInt64)(bits - (UInt64)1));
            count++;
        }

        return (UInt64)count;
    }

    public static UInt64 TrailingZeroCount(UInt64 value)
    {
        UInt64 bits = (UInt64)value;
        if (bits == (UInt64)0)
            return (UInt64)64;

        Int32 count = 0;
        while ((UInt64)((UInt64)(bits >> count) & (UInt64)1) == (UInt64)0)
            count++;

        return (UInt64)count;
    }

    public static UInt64 RotateLeft(UInt64 value, Int32 rotateAmount)
    {
        Int32 n = rotateAmount & 63;
        if (n == 0)
            return value;

        UInt64 bits = (UInt64)value;
        return (UInt64)((UInt64)(bits << n) | (UInt64)(bits >> (64 - n)));
    }

    public static UInt64 RotateRight(UInt64 value, Int32 rotateAmount)
    {
        Int32 n = rotateAmount & 63;
        if (n == 0)
            return value;

        UInt64 bits = (UInt64)value;
        return (UInt64)((UInt64)(bits >> n) | (UInt64)(bits << (64 - n)));
    }

    public Int32 GetByteCount() => 8;

    public Int32 GetShortestBitLength() => 64 - (Int32)LeadingZeroCount(this);

    public static Boolean TryConvertFromChecked<TOther>(TOther value, out UInt64 result)
        where TOther : Numerics.INumberBase<TOther>
    {
        Boolean negative;
        UInt64 magnitude;

        if (!Numerics.IntegerConversions.TryRead(value, out negative, out magnitude))
        {
            result = (UInt64)0;
            return false;
        }

        if (!Numerics.IntegerConversions.Fits(negative, magnitude, 64, false))
            throw new OverflowException();

        UInt64 bits = Numerics.IntegerConversions.Truncate(negative, magnitude);

        result = (UInt64)bits;
        return true;
    }

    public static Boolean TryConvertToChecked<TTarget>(UInt64 value, out TTarget result)
        where TTarget : Numerics.INumberBase<TTarget> => TTarget.TryConvertFromChecked<UInt64>(value, out result);

    public static Boolean TryConvertFromSaturating<TOther>(TOther value, out UInt64 result)
        where TOther : Numerics.INumberBase<TOther>
    {
        Boolean negative;
        UInt64 magnitude;

        if (!Numerics.IntegerConversions.TryRead(value, out negative, out magnitude))
        {
            result = (UInt64)0;
            return false;
        }

        UInt64 bits = Numerics.IntegerConversions.Saturate(negative, magnitude, 64, false);

        result = (UInt64)bits;
        return true;
    }

    public static Boolean TryConvertToSaturating<TTarget>(UInt64 value, out TTarget result)
        where TTarget : Numerics.INumberBase<TTarget> => TTarget.TryConvertFromSaturating<UInt64>(value, out result);

    public static Boolean TryConvertFromTruncating<TOther>(TOther value, out UInt64 result)
        where TOther : Numerics.INumberBase<TOther>
    {
        Boolean negative;
        UInt64 magnitude;

        if (!Numerics.IntegerConversions.TryRead(value, out negative, out magnitude))
        {
            result = (UInt64)0;
            return false;
        }

        UInt64 bits = Numerics.IntegerConversions.Truncate(negative, magnitude);

        result = (UInt64)bits;
        return true;
    }

    public static Boolean TryConvertToTruncating<TTarget>(UInt64 value, out TTarget result)
        where TTarget : Numerics.INumberBase<TTarget> => TTarget.TryConvertFromTruncating<UInt64>(value, out result);

    // ---- end of generic math ----
}
