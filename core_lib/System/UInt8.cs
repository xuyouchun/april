
/*
 *  System.dll
 *
 *  UInt8: the members of the type (the declaration is in +types.cs).
 */

////////// ////////// ////////// ////////// //////////

public partial struct UInt8
{
    public Int32 CompareTo(UInt8 other) => Primitives.Compare(this, other);

    public Boolean Equals(UInt8 other) => Primitives.Same(this, other);

    public Boolean Equals(Object? obj) => obj is UInt8 && Primitives.Same(this, (UInt8)obj);

    public Int32 GetHashCode() => Primitives.Hash(this);

    public String ToString() => Internal.UInt64_ToString(this);

    public String ToString(String? format) => NumberFormat.FormatUnsigned((UInt64)this, 8, format, Globalization.NumberFormatInfo.CurrentInfo);

    public String ToString(String? format, IFormatProvider? provider)
        => NumberFormat.FormatUnsigned((UInt64)this, 8, format, Globalization.NumberFormatInfo.GetInstance(provider));

    // ---- generic math: made by tools/gen_numerics.py, do not edit it here ----

    public static UInt8 operator +(UInt8 left, UInt8 right) => (UInt8)((Int32)left + (Int32)right);

    public static UInt8 operator -(UInt8 left, UInt8 right) => (UInt8)((Int32)left - (Int32)right);

    public static UInt8 operator *(UInt8 left, UInt8 right) => (UInt8)((Int32)left * (Int32)right);

    public static UInt8 operator /(UInt8 left, UInt8 right) => (UInt8)((Int32)left / (Int32)right);

    public static UInt8 operator %(UInt8 left, UInt8 right) => (UInt8)((Int32)left % (Int32)right);

    public static UInt8 operator +(UInt8 value) => value;

    public static UInt8 operator -(UInt8 value) => (UInt8)(-(Int32)value);

    public static UInt8 operator ++(UInt8 value) => (UInt8)((Int32)value + 1);

    public static UInt8 operator --(UInt8 value) => (UInt8)((Int32)value - 1);

    public static UInt8 operator &(UInt8 left, UInt8 right) => (UInt8)((Int32)left & (Int32)right);

    public static UInt8 operator |(UInt8 left, UInt8 right) => (UInt8)((Int32)left | (Int32)right);

    public static UInt8 operator ^(UInt8 left, UInt8 right) => (UInt8)((Int32)left ^ (Int32)right);

    public static UInt8 operator ~(UInt8 value) => (UInt8)(~(Int32)value);

    public static UInt8 operator <<(UInt8 value, Int32 shiftAmount) => (UInt8)((Int32)value << (shiftAmount & 31));

    public static UInt8 operator >>(UInt8 value, Int32 shiftAmount) => (UInt8)((Int32)value >> (shiftAmount & 31));

    public static Boolean operator ==(UInt8 left, UInt8 right) => (Int32)left == (Int32)right;

    public static Boolean operator !=(UInt8 left, UInt8 right) => (Int32)left != (Int32)right;

    public static Boolean operator <(UInt8 left, UInt8 right) => (Int32)left < (Int32)right;

    public static Boolean operator >(UInt8 left, UInt8 right) => (Int32)left > (Int32)right;

    public static Boolean operator <=(UInt8 left, UInt8 right) => (Int32)left <= (Int32)right;

    public static Boolean operator >=(UInt8 left, UInt8 right) => (Int32)left >= (Int32)right;

    public static UInt8 operator checked +(UInt8 left, UInt8 right) => checked((UInt8)((Int32)left + (Int32)right));

    public static UInt8 operator checked -(UInt8 left, UInt8 right) => checked((UInt8)((Int32)left - (Int32)right));

    public static UInt8 operator checked *(UInt8 left, UInt8 right) => checked((UInt8)((Int32)left * (Int32)right));

    public static UInt8 operator checked /(UInt8 left, UInt8 right) => checked((UInt8)((Int32)left / (Int32)right));

    public static UInt8 operator checked -(UInt8 value) => checked((UInt8)(-(Int32)value));

    public static UInt8 operator checked ++(UInt8 value) => checked((UInt8)((Int32)value + 1));

    public static UInt8 operator checked --(UInt8 value) => checked((UInt8)((Int32)value - 1));

    public static UInt8 MinValue => (UInt8)0;

    public static UInt8 MaxValue => (UInt8)255;

    public static UInt8 Zero => (UInt8)0;

    public static UInt8 One => (UInt8)1;

    public static Int32 Radix => 2;

    public static UInt8 AdditiveIdentity => (UInt8)0;

    public static UInt8 MultiplicativeIdentity => (UInt8)1;

    public static UInt8 AllBitsSet => (UInt8)255;

    public static UInt8 Abs(UInt8 value) => value;

    public static Boolean IsCanonical(UInt8 value) => true;

    public static Boolean IsComplexNumber(UInt8 value) => false;

    public static Boolean IsEvenInteger(UInt8 value) => ((Int32)value & 1) == 0;

    public static Boolean IsFinite(UInt8 value) => true;

    public static Boolean IsImaginaryNumber(UInt8 value) => false;

    public static Boolean IsInfinity(UInt8 value) => false;

    public static Boolean IsInteger(UInt8 value) => true;

    public static Boolean IsNaN(UInt8 value) => false;

    public static Boolean IsNegative(UInt8 value) => false;

    public static Boolean IsNegativeInfinity(UInt8 value) => false;

    public static Boolean IsNormal(UInt8 value) => (Int32)value != 0;

    public static Boolean IsOddInteger(UInt8 value) => ((Int32)value & 1) != 0;

    public static Boolean IsPositive(UInt8 value) => true;

    public static Boolean IsPositiveInfinity(UInt8 value) => false;

    public static Boolean IsRealNumber(UInt8 value) => true;

    public static Boolean IsSubnormal(UInt8 value) => false;

    public static Boolean IsZero(UInt8 value) => (Int32)value == 0;

    public static UInt8 MaxMagnitude(UInt8 x, UInt8 y) => (Int32)x > (Int32)y? x : y;

    public static UInt8 MaxMagnitudeNumber(UInt8 x, UInt8 y) => (Int32)x > (Int32)y? x : y;

    public static UInt8 MinMagnitude(UInt8 x, UInt8 y) => (Int32)x < (Int32)y? x : y;

    public static UInt8 MinMagnitudeNumber(UInt8 x, UInt8 y) => (Int32)x < (Int32)y? x : y;

    public static Int32 Sign(UInt8 value) => (Int32)value == 0? 0 : 1;

    public static Boolean IsPow2(UInt8 value) => (Int32)value != 0 && ((Int32)value & ((Int32)value - 1)) == 0;

    public static UInt8 Log2(UInt8 value) => (UInt8)(8 - 1 - (Int32)LeadingZeroCount((UInt8)((Int32)value | 1)));

    public static Tuple<UInt8, UInt8> DivRem(UInt8 left, UInt8 right) =>
        new Tuple<UInt8, UInt8>((UInt8)((Int32)left / (Int32)right), (UInt8)((Int32)left % (Int32)right));

    public static UInt8 LeadingZeroCount(UInt8 value)
    {
        UInt32 bits = (UInt32)value;
        Int32 count = 0;

        for (Int32 bit = 8 - 1; bit >= 0; bit--)
        {
            if ((UInt32)((UInt32)(bits >> bit) & (UInt32)1) != (UInt32)0)
                break;

            count++;
        }

        return (UInt8)count;
    }

    public static UInt8 PopCount(UInt8 value)
    {
        UInt32 bits = (UInt32)value;
        Int32 count = 0;

        while (bits != (UInt32)0)
        {
            bits = (UInt32)(bits & (UInt32)(bits - (UInt32)1));
            count++;
        }

        return (UInt8)count;
    }

    public static UInt8 TrailingZeroCount(UInt8 value)
    {
        UInt32 bits = (UInt32)value;
        if (bits == (UInt32)0)
            return (UInt8)8;

        Int32 count = 0;
        while ((UInt32)((UInt32)(bits >> count) & (UInt32)1) == (UInt32)0)
            count++;

        return (UInt8)count;
    }

    public static UInt8 RotateLeft(UInt8 value, Int32 rotateAmount)
    {
        Int32 n = rotateAmount & 7;
        if (n == 0)
            return value;

        UInt32 bits = (UInt32)value;
        return (UInt8)((UInt32)(bits << n) | (UInt32)(bits >> (8 - n)));
    }

    public static UInt8 RotateRight(UInt8 value, Int32 rotateAmount)
    {
        Int32 n = rotateAmount & 7;
        if (n == 0)
            return value;

        UInt32 bits = (UInt32)value;
        return (UInt8)((UInt32)(bits >> n) | (UInt32)(bits << (8 - n)));
    }

    public Int32 GetByteCount() => 1;

    public Int32 GetShortestBitLength() => 8 - (Int32)LeadingZeroCount(this);

    public static Boolean TryConvertFromChecked<TOther>(TOther value, out UInt8 result)
        where TOther : Numerics.INumberBase<TOther>
    {
        Boolean negative;
        UInt64 magnitude;

        if (!Numerics.IntegerConversions.TryRead(value, out negative, out magnitude))
        {
            result = (UInt8)0;
            return false;
        }

        if (!Numerics.IntegerConversions.Fits(negative, magnitude, 8, false))
            throw new OverflowException();

        UInt64 bits = Numerics.IntegerConversions.Truncate(negative, magnitude);

        result = (UInt8)bits;
        return true;
    }

    public static Boolean TryConvertToChecked<TTarget>(UInt8 value, out TTarget result)
        where TTarget : Numerics.INumberBase<TTarget> => TTarget.TryConvertFromChecked<UInt8>(value, out result);

    public static Boolean TryConvertFromSaturating<TOther>(TOther value, out UInt8 result)
        where TOther : Numerics.INumberBase<TOther>
    {
        Boolean negative;
        UInt64 magnitude;

        if (!Numerics.IntegerConversions.TryRead(value, out negative, out magnitude))
        {
            result = (UInt8)0;
            return false;
        }

        UInt64 bits = Numerics.IntegerConversions.Saturate(negative, magnitude, 8, false);

        result = (UInt8)bits;
        return true;
    }

    public static Boolean TryConvertToSaturating<TTarget>(UInt8 value, out TTarget result)
        where TTarget : Numerics.INumberBase<TTarget> => TTarget.TryConvertFromSaturating<UInt8>(value, out result);

    public static Boolean TryConvertFromTruncating<TOther>(TOther value, out UInt8 result)
        where TOther : Numerics.INumberBase<TOther>
    {
        Boolean negative;
        UInt64 magnitude;

        if (!Numerics.IntegerConversions.TryRead(value, out negative, out magnitude))
        {
            result = (UInt8)0;
            return false;
        }

        UInt64 bits = Numerics.IntegerConversions.Truncate(negative, magnitude);

        result = (UInt8)bits;
        return true;
    }

    public static Boolean TryConvertToTruncating<TTarget>(UInt8 value, out TTarget result)
        where TTarget : Numerics.INumberBase<TTarget> => TTarget.TryConvertFromTruncating<UInt8>(value, out result);

    // ---- end of generic math ----
}
