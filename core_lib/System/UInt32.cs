
/*
 *  System.dll
 *
 *  UInt32: the members of the type (the declaration is in +types.cs).
 */

////////// ////////// ////////// ////////// //////////

public partial struct UInt32
{
    public Int32 CompareTo(UInt32 other) => Primitives.Compare(this, other);

    public Boolean Equals(UInt32 other) => Primitives.Same(this, other);

    public Boolean Equals(Object? obj) => obj is UInt32 && Primitives.Same(this, (UInt32)obj);

    public Int32 GetHashCode() => Primitives.Hash(this);

    public String ToString() => Internal.UInt64_ToString(this);

    public String ToString(String? format) => NumberFormat.FormatUnsigned((UInt64)this, 32, format, Globalization.NumberFormatInfo.CurrentInfo);

    public String ToString(String? format, IFormatProvider? provider)
        => NumberFormat.FormatUnsigned((UInt64)this, 32, format, Globalization.NumberFormatInfo.GetInstance(provider));

    // ---- generic math: made by tools/gen_numerics.py, do not edit it here ----

    public static UInt32 operator +(UInt32 left, UInt32 right) => (UInt32)(left + right);

    public static UInt32 operator -(UInt32 left, UInt32 right) => (UInt32)(left - right);

    public static UInt32 operator *(UInt32 left, UInt32 right) => (UInt32)(left * right);

    public static UInt32 operator /(UInt32 left, UInt32 right) => (UInt32)(left / right);

    public static UInt32 operator %(UInt32 left, UInt32 right) => (UInt32)(left % right);

    public static UInt32 operator +(UInt32 value) => value;

    public static UInt32 operator -(UInt32 value) => (UInt32)(~value + (UInt32)1);

    public static UInt32 operator ++(UInt32 value) => (UInt32)(value + (UInt32)1);

    public static UInt32 operator --(UInt32 value) => (UInt32)(value - (UInt32)1);

    public static UInt32 operator &(UInt32 left, UInt32 right) => (UInt32)(left & right);

    public static UInt32 operator |(UInt32 left, UInt32 right) => (UInt32)(left | right);

    public static UInt32 operator ^(UInt32 left, UInt32 right) => (UInt32)(left ^ right);

    public static UInt32 operator ~(UInt32 value) => (UInt32)(~value);

    public static UInt32 operator <<(UInt32 value, Int32 shiftAmount) => (UInt32)(value << (shiftAmount & 31));

    public static UInt32 operator >>(UInt32 value, Int32 shiftAmount) => (UInt32)(value >> (shiftAmount & 31));

    public static Boolean operator ==(UInt32 left, UInt32 right) => left == right;

    public static Boolean operator !=(UInt32 left, UInt32 right) => left != right;

    public static Boolean operator <(UInt32 left, UInt32 right) => left < right;

    public static Boolean operator >(UInt32 left, UInt32 right) => left > right;

    public static Boolean operator <=(UInt32 left, UInt32 right) => left <= right;

    public static Boolean operator >=(UInt32 left, UInt32 right) => left >= right;

    public static UInt32 operator checked +(UInt32 left, UInt32 right) => checked((UInt32)(left + right));

    public static UInt32 operator checked -(UInt32 left, UInt32 right) => checked((UInt32)(left - right));

    public static UInt32 operator checked *(UInt32 left, UInt32 right) => checked((UInt32)(left * right));

    public static UInt32 operator checked /(UInt32 left, UInt32 right) => checked((UInt32)(left / right));

    public static UInt32 operator checked -(UInt32 value) => checked((UInt32)(-(Int64)value));

    public static UInt32 operator checked ++(UInt32 value) => checked((UInt32)(value + (UInt32)1));

    public static UInt32 operator checked --(UInt32 value) => checked((UInt32)(value - (UInt32)1));

    public static UInt32 MinValue => (UInt32)0;

    public static UInt32 MaxValue => (UInt32)4294967295;

    public static UInt32 Zero => (UInt32)0;

    public static UInt32 One => (UInt32)1;

    public static Int32 Radix => 2;

    public static UInt32 AdditiveIdentity => (UInt32)0;

    public static UInt32 MultiplicativeIdentity => (UInt32)1;

    public static UInt32 AllBitsSet => (UInt32)4294967295;

    public static UInt32 Abs(UInt32 value) => value;

    public static Boolean IsCanonical(UInt32 value) => true;

    public static Boolean IsComplexNumber(UInt32 value) => false;

    public static Boolean IsEvenInteger(UInt32 value) => (value & (UInt32)1) == (UInt32)0;

    public static Boolean IsFinite(UInt32 value) => true;

    public static Boolean IsImaginaryNumber(UInt32 value) => false;

    public static Boolean IsInfinity(UInt32 value) => false;

    public static Boolean IsInteger(UInt32 value) => true;

    public static Boolean IsNaN(UInt32 value) => false;

    public static Boolean IsNegative(UInt32 value) => false;

    public static Boolean IsNegativeInfinity(UInt32 value) => false;

    public static Boolean IsNormal(UInt32 value) => value != (UInt32)0;

    public static Boolean IsOddInteger(UInt32 value) => (value & (UInt32)1) != (UInt32)0;

    public static Boolean IsPositive(UInt32 value) => true;

    public static Boolean IsPositiveInfinity(UInt32 value) => false;

    public static Boolean IsRealNumber(UInt32 value) => true;

    public static Boolean IsSubnormal(UInt32 value) => false;

    public static Boolean IsZero(UInt32 value) => value == (UInt32)0;

    public static UInt32 MaxMagnitude(UInt32 x, UInt32 y) => x > y? x : y;

    public static UInt32 MaxMagnitudeNumber(UInt32 x, UInt32 y) => x > y? x : y;

    public static UInt32 MinMagnitude(UInt32 x, UInt32 y) => x < y? x : y;

    public static UInt32 MinMagnitudeNumber(UInt32 x, UInt32 y) => x < y? x : y;

    public static Int32 Sign(UInt32 value) => value == (UInt32)0? 0 : 1;

    public static Boolean IsPow2(UInt32 value) => value != (UInt32)0 && (value & (value - (UInt32)1)) == (UInt32)0;

    public static UInt32 Log2(UInt32 value) => (UInt32)(32 - 1 - (Int32)LeadingZeroCount((UInt32)(value | (UInt32)1)));

    public static Tuple<UInt32, UInt32> DivRem(UInt32 left, UInt32 right) =>
        new Tuple<UInt32, UInt32>((UInt32)(left / right), (UInt32)(left % right));

    public static UInt32 LeadingZeroCount(UInt32 value)
    {
        UInt32 bits = (UInt32)value;
        Int32 count = 0;

        for (Int32 bit = 32 - 1; bit >= 0; bit--)
        {
            if ((UInt32)((UInt32)(bits >> bit) & (UInt32)1) != (UInt32)0)
                break;

            count++;
        }

        return (UInt32)count;
    }

    public static UInt32 PopCount(UInt32 value)
    {
        UInt32 bits = (UInt32)value;
        Int32 count = 0;

        while (bits != (UInt32)0)
        {
            bits = (UInt32)(bits & (UInt32)(bits - (UInt32)1));
            count++;
        }

        return (UInt32)count;
    }

    public static UInt32 TrailingZeroCount(UInt32 value)
    {
        UInt32 bits = (UInt32)value;
        if (bits == (UInt32)0)
            return (UInt32)32;

        Int32 count = 0;
        while ((UInt32)((UInt32)(bits >> count) & (UInt32)1) == (UInt32)0)
            count++;

        return (UInt32)count;
    }

    public static UInt32 RotateLeft(UInt32 value, Int32 rotateAmount)
    {
        Int32 n = rotateAmount & 31;
        if (n == 0)
            return value;

        UInt32 bits = (UInt32)value;
        return (UInt32)((UInt32)(bits << n) | (UInt32)(bits >> (32 - n)));
    }

    public static UInt32 RotateRight(UInt32 value, Int32 rotateAmount)
    {
        Int32 n = rotateAmount & 31;
        if (n == 0)
            return value;

        UInt32 bits = (UInt32)value;
        return (UInt32)((UInt32)(bits >> n) | (UInt32)(bits << (32 - n)));
    }

    public Int32 GetByteCount() => 4;

    public Int32 GetShortestBitLength() => 32 - (Int32)LeadingZeroCount(this);

    public static Boolean TryConvertFromChecked<TOther>(TOther value, out UInt32 result)
        where TOther : Numerics.INumberBase<TOther>
    {
        Boolean negative;
        UInt64 magnitude;

        if (!Numerics.IntegerConversions.TryRead(value, out negative, out magnitude))
        {
            result = (UInt32)0;
            return false;
        }

        if (!Numerics.IntegerConversions.Fits(negative, magnitude, 32, false))
            throw new OverflowException();

        UInt64 bits = Numerics.IntegerConversions.Truncate(negative, magnitude);

        result = (UInt32)bits;
        return true;
    }

    public static Boolean TryConvertToChecked<TTarget>(UInt32 value, out TTarget result)
        where TTarget : Numerics.INumberBase<TTarget> => TTarget.TryConvertFromChecked<UInt32>(value, out result);

    public static Boolean TryConvertFromSaturating<TOther>(TOther value, out UInt32 result)
        where TOther : Numerics.INumberBase<TOther>
    {
        Boolean negative;
        UInt64 magnitude;

        if (!Numerics.IntegerConversions.TryRead(value, out negative, out magnitude))
        {
            result = (UInt32)0;
            return false;
        }

        UInt64 bits = Numerics.IntegerConversions.Saturate(negative, magnitude, 32, false);

        result = (UInt32)bits;
        return true;
    }

    public static Boolean TryConvertToSaturating<TTarget>(UInt32 value, out TTarget result)
        where TTarget : Numerics.INumberBase<TTarget> => TTarget.TryConvertFromSaturating<UInt32>(value, out result);

    public static Boolean TryConvertFromTruncating<TOther>(TOther value, out UInt32 result)
        where TOther : Numerics.INumberBase<TOther>
    {
        Boolean negative;
        UInt64 magnitude;

        if (!Numerics.IntegerConversions.TryRead(value, out negative, out magnitude))
        {
            result = (UInt32)0;
            return false;
        }

        UInt64 bits = Numerics.IntegerConversions.Truncate(negative, magnitude);

        result = (UInt32)bits;
        return true;
    }

    public static Boolean TryConvertToTruncating<TTarget>(UInt32 value, out TTarget result)
        where TTarget : Numerics.INumberBase<TTarget> => TTarget.TryConvertFromTruncating<UInt32>(value, out result);

    // ---- end of generic math ----
}
