
/*
 *  System.dll
 *
 *  UInt16: the members of the type (the declaration is in +types.cs).
 */

////////// ////////// ////////// ////////// //////////

public partial struct UInt16
{
    public Int32 CompareTo(UInt16 other) => Primitives.Compare(this, other);

    public Boolean Equals(UInt16 other) => Primitives.Same(this, other);

    public Boolean Equals(Object? obj) => obj is UInt16 && Primitives.Same(this, (UInt16)obj);

    public Int32 GetHashCode() => Primitives.Hash(this);

    public String ToString() => Internal.UInt64_ToString(this);

    public String ToString(String? format) => NumberFormat.FormatUnsigned((UInt64)this, 16, format, Globalization.NumberFormatInfo.CurrentInfo);

    public String ToString(String? format, IFormatProvider? provider)
        => NumberFormat.FormatUnsigned((UInt64)this, 16, format, Globalization.NumberFormatInfo.GetInstance(provider));

    // The number of a text: spaces, a sign and digits (see NumberParse). FormatException, OverflowException.
    public static UInt16 Parse(String text) => Parse(text, null);

    public static UInt16 Parse(String text, IFormatProvider? provider)
    {
        UInt64 value;
        Int32 status = NumberParse.Unsigned(text, Globalization.NumberFormatInfo.GetInstance(provider), (UInt64)65535, out value);
        if (status != 0)
            NumberParse.Throw(status, text, "UInt16");

        return (UInt16)value;
    }

    public static Boolean TryParse(String? text, out UInt16 result) => TryParse(text, null, out result);

    public static Boolean TryParse(String? text, IFormatProvider? provider, out UInt16 result)
    {
        UInt64 value;
        Int32 status = NumberParse.Unsigned(text, Globalization.NumberFormatInfo.GetInstance(provider), (UInt64)65535, out value);

        result = (UInt16)value;
        return status == 0;
    }

    // ---- generic math: made by tools/gen_numerics.py, do not edit it here ----

    public static UInt16 operator +(UInt16 left, UInt16 right) => (UInt16)((Int32)left + (Int32)right);

    public static UInt16 operator -(UInt16 left, UInt16 right) => (UInt16)((Int32)left - (Int32)right);

    public static UInt16 operator *(UInt16 left, UInt16 right) => (UInt16)((Int32)left * (Int32)right);

    public static UInt16 operator /(UInt16 left, UInt16 right) => (UInt16)((Int32)left / (Int32)right);

    public static UInt16 operator %(UInt16 left, UInt16 right) => (UInt16)((Int32)left % (Int32)right);

    public static UInt16 operator +(UInt16 value) => value;

    public static UInt16 operator -(UInt16 value) => (UInt16)(-(Int32)value);

    public static UInt16 operator ++(UInt16 value) => (UInt16)((Int32)value + 1);

    public static UInt16 operator --(UInt16 value) => (UInt16)((Int32)value - 1);

    public static UInt16 operator &(UInt16 left, UInt16 right) => (UInt16)((Int32)left & (Int32)right);

    public static UInt16 operator |(UInt16 left, UInt16 right) => (UInt16)((Int32)left | (Int32)right);

    public static UInt16 operator ^(UInt16 left, UInt16 right) => (UInt16)((Int32)left ^ (Int32)right);

    public static UInt16 operator ~(UInt16 value) => (UInt16)(~(Int32)value);

    public static UInt16 operator <<(UInt16 value, Int32 shiftAmount) => (UInt16)((Int32)value << (shiftAmount & 31));

    public static UInt16 operator >>(UInt16 value, Int32 shiftAmount) => (UInt16)((Int32)value >> (shiftAmount & 31));

    public static Boolean operator ==(UInt16 left, UInt16 right) => (Int32)left == (Int32)right;

    public static Boolean operator !=(UInt16 left, UInt16 right) => (Int32)left != (Int32)right;

    public static Boolean operator <(UInt16 left, UInt16 right) => (Int32)left < (Int32)right;

    public static Boolean operator >(UInt16 left, UInt16 right) => (Int32)left > (Int32)right;

    public static Boolean operator <=(UInt16 left, UInt16 right) => (Int32)left <= (Int32)right;

    public static Boolean operator >=(UInt16 left, UInt16 right) => (Int32)left >= (Int32)right;

    public static UInt16 operator checked +(UInt16 left, UInt16 right) => checked((UInt16)((Int32)left + (Int32)right));

    public static UInt16 operator checked -(UInt16 left, UInt16 right) => checked((UInt16)((Int32)left - (Int32)right));

    public static UInt16 operator checked *(UInt16 left, UInt16 right) => checked((UInt16)((Int32)left * (Int32)right));

    public static UInt16 operator checked /(UInt16 left, UInt16 right) => checked((UInt16)((Int32)left / (Int32)right));

    public static UInt16 operator checked -(UInt16 value) => checked((UInt16)(-(Int32)value));

    public static UInt16 operator checked ++(UInt16 value) => checked((UInt16)((Int32)value + 1));

    public static UInt16 operator checked --(UInt16 value) => checked((UInt16)((Int32)value - 1));

    public static UInt16 MinValue => (UInt16)0;

    public static UInt16 MaxValue => (UInt16)65535;

    public static UInt16 Zero => (UInt16)0;

    public static UInt16 One => (UInt16)1;

    public static Int32 Radix => 2;

    public static UInt16 AdditiveIdentity => (UInt16)0;

    public static UInt16 MultiplicativeIdentity => (UInt16)1;

    public static UInt16 AllBitsSet => (UInt16)65535;

    public static UInt16 Abs(UInt16 value) => value;

    public static Boolean IsCanonical(UInt16 value) => true;

    public static Boolean IsComplexNumber(UInt16 value) => false;

    public static Boolean IsEvenInteger(UInt16 value) => ((Int32)value & 1) == 0;

    public static Boolean IsFinite(UInt16 value) => true;

    public static Boolean IsImaginaryNumber(UInt16 value) => false;

    public static Boolean IsInfinity(UInt16 value) => false;

    public static Boolean IsInteger(UInt16 value) => true;

    public static Boolean IsNaN(UInt16 value) => false;

    public static Boolean IsNegative(UInt16 value) => false;

    public static Boolean IsNegativeInfinity(UInt16 value) => false;

    public static Boolean IsNormal(UInt16 value) => (Int32)value != 0;

    public static Boolean IsOddInteger(UInt16 value) => ((Int32)value & 1) != 0;

    public static Boolean IsPositive(UInt16 value) => true;

    public static Boolean IsPositiveInfinity(UInt16 value) => false;

    public static Boolean IsRealNumber(UInt16 value) => true;

    public static Boolean IsSubnormal(UInt16 value) => false;

    public static Boolean IsZero(UInt16 value) => (Int32)value == 0;

    public static UInt16 MaxMagnitude(UInt16 x, UInt16 y) => (Int32)x > (Int32)y? x : y;

    public static UInt16 MaxMagnitudeNumber(UInt16 x, UInt16 y) => (Int32)x > (Int32)y? x : y;

    public static UInt16 MinMagnitude(UInt16 x, UInt16 y) => (Int32)x < (Int32)y? x : y;

    public static UInt16 MinMagnitudeNumber(UInt16 x, UInt16 y) => (Int32)x < (Int32)y? x : y;

    public static Int32 Sign(UInt16 value) => (Int32)value == 0? 0 : 1;

    public static Boolean IsPow2(UInt16 value) => (Int32)value != 0 && ((Int32)value & ((Int32)value - 1)) == 0;

    public static UInt16 Log2(UInt16 value) => (UInt16)(16 - 1 - (Int32)LeadingZeroCount((UInt16)((Int32)value | 1)));

    public static Tuple<UInt16, UInt16> DivRem(UInt16 left, UInt16 right) =>
        new Tuple<UInt16, UInt16>((UInt16)((Int32)left / (Int32)right), (UInt16)((Int32)left % (Int32)right));

    public static UInt16 LeadingZeroCount(UInt16 value)
    {
        UInt32 bits = (UInt32)value;
        Int32 count = 0;

        for (Int32 bit = 16 - 1; bit >= 0; bit--)
        {
            if ((UInt32)((UInt32)(bits >> bit) & (UInt32)1) != (UInt32)0)
                break;

            count++;
        }

        return (UInt16)count;
    }

    public static UInt16 PopCount(UInt16 value)
    {
        UInt32 bits = (UInt32)value;
        Int32 count = 0;

        while (bits != (UInt32)0)
        {
            bits = (UInt32)(bits & (UInt32)(bits - (UInt32)1));
            count++;
        }

        return (UInt16)count;
    }

    public static UInt16 TrailingZeroCount(UInt16 value)
    {
        UInt32 bits = (UInt32)value;
        if (bits == (UInt32)0)
            return (UInt16)16;

        Int32 count = 0;
        while ((UInt32)((UInt32)(bits >> count) & (UInt32)1) == (UInt32)0)
            count++;

        return (UInt16)count;
    }

    public static UInt16 RotateLeft(UInt16 value, Int32 rotateAmount)
    {
        Int32 n = rotateAmount & 15;
        if (n == 0)
            return value;

        UInt32 bits = (UInt32)value;
        return (UInt16)((UInt32)(bits << n) | (UInt32)(bits >> (16 - n)));
    }

    public static UInt16 RotateRight(UInt16 value, Int32 rotateAmount)
    {
        Int32 n = rotateAmount & 15;
        if (n == 0)
            return value;

        UInt32 bits = (UInt32)value;
        return (UInt16)((UInt32)(bits >> n) | (UInt32)(bits << (16 - n)));
    }

    public Int32 GetByteCount() => 2;

    public Int32 GetShortestBitLength() => 16 - (Int32)LeadingZeroCount(this);

    public static Boolean TryConvertFromChecked<TOther>(TOther value, out UInt16 result)
        where TOther : Numerics.INumberBase<TOther>
    {
        Boolean negative;
        UInt64 magnitude;

        if (!Numerics.IntegerConversions.TryRead(value, out negative, out magnitude))
        {
            result = (UInt16)0;
            return false;
        }

        if (!Numerics.IntegerConversions.Fits(negative, magnitude, 16, false))
            throw new OverflowException();

        UInt64 bits = Numerics.IntegerConversions.Truncate(negative, magnitude);

        result = (UInt16)bits;
        return true;
    }

    public static Boolean TryConvertToChecked<TTarget>(UInt16 value, out TTarget result)
        where TTarget : Numerics.INumberBase<TTarget> => TTarget.TryConvertFromChecked<UInt16>(value, out result);

    public static Boolean TryConvertFromSaturating<TOther>(TOther value, out UInt16 result)
        where TOther : Numerics.INumberBase<TOther>
    {
        Boolean negative;
        UInt64 magnitude;

        if (!Numerics.IntegerConversions.TryRead(value, out negative, out magnitude))
        {
            result = (UInt16)0;
            return false;
        }

        UInt64 bits = Numerics.IntegerConversions.Saturate(negative, magnitude, 16, false);

        result = (UInt16)bits;
        return true;
    }

    public static Boolean TryConvertToSaturating<TTarget>(UInt16 value, out TTarget result)
        where TTarget : Numerics.INumberBase<TTarget> => TTarget.TryConvertFromSaturating<UInt16>(value, out result);

    public static Boolean TryConvertFromTruncating<TOther>(TOther value, out UInt16 result)
        where TOther : Numerics.INumberBase<TOther>
    {
        Boolean negative;
        UInt64 magnitude;

        if (!Numerics.IntegerConversions.TryRead(value, out negative, out magnitude))
        {
            result = (UInt16)0;
            return false;
        }

        UInt64 bits = Numerics.IntegerConversions.Truncate(negative, magnitude);

        result = (UInt16)bits;
        return true;
    }

    public static Boolean TryConvertToTruncating<TTarget>(UInt16 value, out TTarget result)
        where TTarget : Numerics.INumberBase<TTarget> => TTarget.TryConvertFromTruncating<UInt16>(value, out result);

    // ---- end of generic math ----
}
