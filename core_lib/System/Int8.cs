
/*
 *  System.dll
 *
 *  Int8: the members of the type (the declaration is in +types.cs).
 */

////////// ////////// ////////// ////////// //////////

public partial struct Int8
{
    public Int32 CompareTo(Int8 other) => Primitives.Compare(this, other);

    public Boolean Equals(Int8 other) => Primitives.Same(this, other);

    public Boolean Equals(Object? obj) => obj is Int8 && Primitives.Same(this, (Int8)obj);

    public Int32 GetHashCode() => Primitives.Hash(this);

    public String ToString() => Internal.Int64_ToString(this);

    public String ToString(String? format) => NumberFormat.FormatSigned((Int64)this, 8, format, Globalization.NumberFormatInfo.CurrentInfo);

    public String ToString(String? format, IFormatProvider? provider)
        => NumberFormat.FormatSigned((Int64)this, 8, format, Globalization.NumberFormatInfo.GetInstance(provider));

    // ---- generic math: made by tools/gen_numerics.py, do not edit it here ----

    public static Int8 operator +(Int8 left, Int8 right) => (Int8)((Int32)left + (Int32)right);

    public static Int8 operator -(Int8 left, Int8 right) => (Int8)((Int32)left - (Int32)right);

    public static Int8 operator *(Int8 left, Int8 right) => (Int8)((Int32)left * (Int32)right);

    public static Int8 operator /(Int8 left, Int8 right) => (Int8)((Int32)left / (Int32)right);

    public static Int8 operator %(Int8 left, Int8 right) => (Int8)((Int32)left % (Int32)right);

    public static Int8 operator +(Int8 value) => value;

    public static Int8 operator -(Int8 value) => (Int8)(-(Int32)value);

    public static Int8 operator ++(Int8 value) => (Int8)((Int32)value + 1);

    public static Int8 operator --(Int8 value) => (Int8)((Int32)value - 1);

    public static Int8 operator &(Int8 left, Int8 right) => (Int8)((Int32)left & (Int32)right);

    public static Int8 operator |(Int8 left, Int8 right) => (Int8)((Int32)left | (Int32)right);

    public static Int8 operator ^(Int8 left, Int8 right) => (Int8)((Int32)left ^ (Int32)right);

    public static Int8 operator ~(Int8 value) => (Int8)(~(Int32)value);

    public static Int8 operator <<(Int8 value, Int32 shiftAmount) => (Int8)((Int32)value << (shiftAmount & 31));

    public static Int8 operator >>(Int8 value, Int32 shiftAmount) => (Int8)((Int32)value >> (shiftAmount & 31));

    public static Boolean operator ==(Int8 left, Int8 right) => (Int32)left == (Int32)right;

    public static Boolean operator !=(Int8 left, Int8 right) => (Int32)left != (Int32)right;

    public static Boolean operator <(Int8 left, Int8 right) => (Int32)left < (Int32)right;

    public static Boolean operator >(Int8 left, Int8 right) => (Int32)left > (Int32)right;

    public static Boolean operator <=(Int8 left, Int8 right) => (Int32)left <= (Int32)right;

    public static Boolean operator >=(Int8 left, Int8 right) => (Int32)left >= (Int32)right;

    public static Int8 operator checked +(Int8 left, Int8 right) => checked((Int8)((Int32)left + (Int32)right));

    public static Int8 operator checked -(Int8 left, Int8 right) => checked((Int8)((Int32)left - (Int32)right));

    public static Int8 operator checked *(Int8 left, Int8 right) => checked((Int8)((Int32)left * (Int32)right));

    public static Int8 operator checked /(Int8 left, Int8 right) => checked((Int8)((Int32)left / (Int32)right));

    public static Int8 operator checked -(Int8 value) => checked((Int8)(-(Int32)value));

    public static Int8 operator checked ++(Int8 value) => checked((Int8)((Int32)value + 1));

    public static Int8 operator checked --(Int8 value) => checked((Int8)((Int32)value - 1));

    public static Int8 MinValue => (Int8)(-128);

    public static Int8 MaxValue => (Int8)127;

    public static Int8 Zero => (Int8)0;

    public static Int8 One => (Int8)1;

    public static Int32 Radix => 2;

    public static Int8 AdditiveIdentity => (Int8)0;

    public static Int8 MultiplicativeIdentity => (Int8)1;

    public static Int8 AllBitsSet => (Int8)(-1);

    public static Int8 NegativeOne => (Int8)(-1);

    public static Int8 Abs(Int8 value)
    {
        if ((Int32)value < 0)
        {
            Int8 result = (Int8)(-(Int32)value);
            if ((Int32)result < 0)
                throw new OverflowException();

            return result;
        }

        return value;
    }

    public static Boolean IsCanonical(Int8 value) => true;

    public static Boolean IsComplexNumber(Int8 value) => false;

    public static Boolean IsEvenInteger(Int8 value) => ((Int32)value & 1) == 0;

    public static Boolean IsFinite(Int8 value) => true;

    public static Boolean IsImaginaryNumber(Int8 value) => false;

    public static Boolean IsInfinity(Int8 value) => false;

    public static Boolean IsInteger(Int8 value) => true;

    public static Boolean IsNaN(Int8 value) => false;

    public static Boolean IsNegative(Int8 value) => (Int32)value < 0;

    public static Boolean IsNegativeInfinity(Int8 value) => false;

    public static Boolean IsNormal(Int8 value) => (Int32)value != 0;

    public static Boolean IsOddInteger(Int8 value) => ((Int32)value & 1) != 0;

    public static Boolean IsPositive(Int8 value) => (Int32)value >= 0;

    public static Boolean IsPositiveInfinity(Int8 value) => false;

    public static Boolean IsRealNumber(Int8 value) => true;

    public static Boolean IsSubnormal(Int8 value) => false;

    public static Boolean IsZero(Int8 value) => (Int32)value == 0;

    public static Int8 MaxMagnitude(Int8 x, Int8 y)
    {
        Int8 ax = x, ay = y;

        if ((Int32)x < 0)
        {
            ax = (Int8)(-(Int32)x);
            if ((Int32)ax < 0)
                return x;
        }

        if ((Int32)y < 0)
        {
            ay = (Int8)(-(Int32)y);
            if ((Int32)ay < 0)
                return y;
        }

        if ((Int32)ax > (Int32)ay)
            return x;

        if ((Int32)ax == (Int32)ay)
            return (Int32)x < 0? y : x;

        return y;
    }

    public static Int8 MaxMagnitudeNumber(Int8 x, Int8 y) => MaxMagnitude(x, y);

    public static Int8 MinMagnitude(Int8 x, Int8 y)
    {
        Int8 ax = x, ay = y;

        if ((Int32)x < 0)
        {
            ax = (Int8)(-(Int32)x);
            if ((Int32)ax < 0)
                return y;
        }

        if ((Int32)y < 0)
        {
            ay = (Int8)(-(Int32)y);
            if ((Int32)ay < 0)
                return x;
        }

        if ((Int32)ax < (Int32)ay)
            return x;

        if ((Int32)ax == (Int32)ay)
            return (Int32)x < 0? x : y;

        return y;
    }

    public static Int8 MinMagnitudeNumber(Int8 x, Int8 y) => MinMagnitude(x, y);

    public static Int32 Sign(Int8 value) => (Int32)value < 0? -1 : ((Int32)value > 0? 1 : 0);

    public static Boolean IsPow2(Int8 value) => (Int32)value > 0 && (Int32)value != 0 && ((Int32)value & ((Int32)value - 1)) == 0;

    public static Int8 Log2(Int8 value)
    {
        if ((Int32)value < 0)
            throw new OverflowException();

        return (Int8)(8 - 1 - (Int32)LeadingZeroCount((Int8)((Int32)value | 1)));
    }

    public static Tuple<Int8, Int8> DivRem(Int8 left, Int8 right) =>
        new Tuple<Int8, Int8>((Int8)((Int32)left / (Int32)right), (Int8)((Int32)left % (Int32)right));

    public static Int8 LeadingZeroCount(Int8 value)
    {
        UInt32 bits = (UInt32)(UInt8)value;
        Int32 count = 0;

        for (Int32 bit = 8 - 1; bit >= 0; bit--)
        {
            if ((UInt32)((UInt32)(bits >> bit) & (UInt32)1) != (UInt32)0)
                break;

            count++;
        }

        return (Int8)count;
    }

    public static Int8 PopCount(Int8 value)
    {
        UInt32 bits = (UInt32)(UInt8)value;
        Int32 count = 0;

        while (bits != (UInt32)0)
        {
            bits = (UInt32)(bits & (UInt32)(bits - (UInt32)1));
            count++;
        }

        return (Int8)count;
    }

    public static Int8 TrailingZeroCount(Int8 value)
    {
        UInt32 bits = (UInt32)(UInt8)value;
        if (bits == (UInt32)0)
            return (Int8)8;

        Int32 count = 0;
        while ((UInt32)((UInt32)(bits >> count) & (UInt32)1) == (UInt32)0)
            count++;

        return (Int8)count;
    }

    public static Int8 RotateLeft(Int8 value, Int32 rotateAmount)
    {
        Int32 n = rotateAmount & 7;
        if (n == 0)
            return value;

        UInt32 bits = (UInt32)(UInt8)value;
        return (Int8)((UInt32)(bits << n) | (UInt32)(bits >> (8 - n)));
    }

    public static Int8 RotateRight(Int8 value, Int32 rotateAmount)
    {
        Int32 n = rotateAmount & 7;
        if (n == 0)
            return value;

        UInt32 bits = (UInt32)(UInt8)value;
        return (Int8)((UInt32)(bits >> n) | (UInt32)(bits << (8 - n)));
    }

    public Int32 GetByteCount() => 1;

    public Int32 GetShortestBitLength()
    {
        Int8 value = this;

        if ((Int32)value >= 0)
            return 8 - (Int32)LeadingZeroCount(value);

        return 9 - (Int32)LeadingZeroCount((Int8)(~(Int32)value));
    }

    public static Boolean TryConvertFromChecked<TOther>(TOther value, out Int8 result)
        where TOther : Numerics.INumberBase<TOther>
    {
        Boolean negative;
        UInt64 magnitude;

        if (!Numerics.IntegerConversions.TryRead(value, out negative, out magnitude))
        {
            result = (Int8)0;
            return false;
        }

        if (!Numerics.IntegerConversions.Fits(negative, magnitude, 8, true))
            throw new OverflowException();

        UInt64 bits = Numerics.IntegerConversions.Truncate(negative, magnitude);

        result = (Int8)(Int64)bits;
        return true;
    }

    public static Boolean TryConvertToChecked<TTarget>(Int8 value, out TTarget result)
        where TTarget : Numerics.INumberBase<TTarget> => TTarget.TryConvertFromChecked<Int8>(value, out result);

    public static Boolean TryConvertFromSaturating<TOther>(TOther value, out Int8 result)
        where TOther : Numerics.INumberBase<TOther>
    {
        Boolean negative;
        UInt64 magnitude;

        if (!Numerics.IntegerConversions.TryRead(value, out negative, out magnitude))
        {
            result = (Int8)0;
            return false;
        }

        UInt64 bits = Numerics.IntegerConversions.Saturate(negative, magnitude, 8, true);

        result = (Int8)(Int64)bits;
        return true;
    }

    public static Boolean TryConvertToSaturating<TTarget>(Int8 value, out TTarget result)
        where TTarget : Numerics.INumberBase<TTarget> => TTarget.TryConvertFromSaturating<Int8>(value, out result);

    public static Boolean TryConvertFromTruncating<TOther>(TOther value, out Int8 result)
        where TOther : Numerics.INumberBase<TOther>
    {
        Boolean negative;
        UInt64 magnitude;

        if (!Numerics.IntegerConversions.TryRead(value, out negative, out magnitude))
        {
            result = (Int8)0;
            return false;
        }

        UInt64 bits = Numerics.IntegerConversions.Truncate(negative, magnitude);

        result = (Int8)(Int64)bits;
        return true;
    }

    public static Boolean TryConvertToTruncating<TTarget>(Int8 value, out TTarget result)
        where TTarget : Numerics.INumberBase<TTarget> => TTarget.TryConvertFromTruncating<Int8>(value, out result);

    // ---- end of generic math ----
}
