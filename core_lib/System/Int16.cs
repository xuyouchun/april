
/*
 *  System.dll
 *
 *  Int16: the members of the type (the declaration is in +types.cs).
 */

////////// ////////// ////////// ////////// //////////

public partial struct Int16
{
    public Int32 CompareTo(Int16 other) => Primitives.Compare(this, other);

    public Boolean Equals(Int16 other) => Primitives.Same(this, other);

    public Boolean Equals(Object? obj) => obj is Int16 && Primitives.Same(this, (Int16)obj);

    public Int32 GetHashCode() => Primitives.Hash(this);

    public String ToString() => Internal.Int64_ToString(this);

    public String ToString(String? format) => NumberFormat.FormatSigned((Int64)this, 16, format, Globalization.NumberFormatInfo.CurrentInfo);

    public String ToString(String? format, IFormatProvider? provider)
        => NumberFormat.FormatSigned((Int64)this, 16, format, Globalization.NumberFormatInfo.GetInstance(provider));

    // The number of a text: spaces, a sign and digits (see NumberParse). FormatException, OverflowException.
    public static Int16 Parse(String text) => Parse(text, null);

    public static Int16 Parse(String text, IFormatProvider? provider)
    {
        Int64 value;
        Int32 status = NumberParse.Signed(text, Globalization.NumberFormatInfo.GetInstance(provider), -32768, 32767, out value);
        if (status != 0)
            NumberParse.Throw(status, text, "Int16");

        return (Int16)value;
    }

    public static Boolean TryParse(String? text, out Int16 result) => TryParse(text, null, out result);

    public static Boolean TryParse(String? text, IFormatProvider? provider, out Int16 result)
    {
        Int64 value;
        Int32 status = NumberParse.Signed(text, Globalization.NumberFormatInfo.GetInstance(provider), -32768, 32767, out value);

        result = (Int16)value;
        return status == 0;
    }

    // ---- generic math: made by tools/gen_numerics.py, do not edit it here ----

    public static Int16 operator +(Int16 left, Int16 right) => (Int16)((Int32)left + (Int32)right);

    public static Int16 operator -(Int16 left, Int16 right) => (Int16)((Int32)left - (Int32)right);

    public static Int16 operator *(Int16 left, Int16 right) => (Int16)((Int32)left * (Int32)right);

    public static Int16 operator /(Int16 left, Int16 right) => (Int16)((Int32)left / (Int32)right);

    public static Int16 operator %(Int16 left, Int16 right) => (Int16)((Int32)left % (Int32)right);

    public static Int16 operator +(Int16 value) => value;

    public static Int16 operator -(Int16 value) => (Int16)(-(Int32)value);

    public static Int16 operator ++(Int16 value) => (Int16)((Int32)value + 1);

    public static Int16 operator --(Int16 value) => (Int16)((Int32)value - 1);

    public static Int16 operator &(Int16 left, Int16 right) => (Int16)((Int32)left & (Int32)right);

    public static Int16 operator |(Int16 left, Int16 right) => (Int16)((Int32)left | (Int32)right);

    public static Int16 operator ^(Int16 left, Int16 right) => (Int16)((Int32)left ^ (Int32)right);

    public static Int16 operator ~(Int16 value) => (Int16)(~(Int32)value);

    public static Int16 operator <<(Int16 value, Int32 shiftAmount) => (Int16)((Int32)value << (shiftAmount & 31));

    public static Int16 operator >>(Int16 value, Int32 shiftAmount) => (Int16)((Int32)value >> (shiftAmount & 31));

    public static Boolean operator ==(Int16 left, Int16 right) => (Int32)left == (Int32)right;

    public static Boolean operator !=(Int16 left, Int16 right) => (Int32)left != (Int32)right;

    public static Boolean operator <(Int16 left, Int16 right) => (Int32)left < (Int32)right;

    public static Boolean operator >(Int16 left, Int16 right) => (Int32)left > (Int32)right;

    public static Boolean operator <=(Int16 left, Int16 right) => (Int32)left <= (Int32)right;

    public static Boolean operator >=(Int16 left, Int16 right) => (Int32)left >= (Int32)right;

    public static Int16 operator checked +(Int16 left, Int16 right) => checked((Int16)((Int32)left + (Int32)right));

    public static Int16 operator checked -(Int16 left, Int16 right) => checked((Int16)((Int32)left - (Int32)right));

    public static Int16 operator checked *(Int16 left, Int16 right) => checked((Int16)((Int32)left * (Int32)right));

    public static Int16 operator checked /(Int16 left, Int16 right) => checked((Int16)((Int32)left / (Int32)right));

    public static Int16 operator checked -(Int16 value) => checked((Int16)(-(Int32)value));

    public static Int16 operator checked ++(Int16 value) => checked((Int16)((Int32)value + 1));

    public static Int16 operator checked --(Int16 value) => checked((Int16)((Int32)value - 1));

    public static Int16 MinValue => (Int16)(-32768);

    public static Int16 MaxValue => (Int16)32767;

    public static Int16 Zero => (Int16)0;

    public static Int16 One => (Int16)1;

    public static Int32 Radix => 2;

    public static Int16 AdditiveIdentity => (Int16)0;

    public static Int16 MultiplicativeIdentity => (Int16)1;

    public static Int16 AllBitsSet => (Int16)(-1);

    public static Int16 NegativeOne => (Int16)(-1);

    public static Int16 Abs(Int16 value)
    {
        if ((Int32)value < 0)
        {
            Int16 result = (Int16)(-(Int32)value);
            if ((Int32)result < 0)
                throw new OverflowException();

            return result;
        }

        return value;
    }

    public static Boolean IsCanonical(Int16 value) => true;

    public static Boolean IsComplexNumber(Int16 value) => false;

    public static Boolean IsEvenInteger(Int16 value) => ((Int32)value & 1) == 0;

    public static Boolean IsFinite(Int16 value) => true;

    public static Boolean IsImaginaryNumber(Int16 value) => false;

    public static Boolean IsInfinity(Int16 value) => false;

    public static Boolean IsInteger(Int16 value) => true;

    public static Boolean IsNaN(Int16 value) => false;

    public static Boolean IsNegative(Int16 value) => (Int32)value < 0;

    public static Boolean IsNegativeInfinity(Int16 value) => false;

    public static Boolean IsNormal(Int16 value) => (Int32)value != 0;

    public static Boolean IsOddInteger(Int16 value) => ((Int32)value & 1) != 0;

    public static Boolean IsPositive(Int16 value) => (Int32)value >= 0;

    public static Boolean IsPositiveInfinity(Int16 value) => false;

    public static Boolean IsRealNumber(Int16 value) => true;

    public static Boolean IsSubnormal(Int16 value) => false;

    public static Boolean IsZero(Int16 value) => (Int32)value == 0;

    public static Int16 MaxMagnitude(Int16 x, Int16 y)
    {
        Int16 ax = x, ay = y;

        if ((Int32)x < 0)
        {
            ax = (Int16)(-(Int32)x);
            if ((Int32)ax < 0)
                return x;
        }

        if ((Int32)y < 0)
        {
            ay = (Int16)(-(Int32)y);
            if ((Int32)ay < 0)
                return y;
        }

        if ((Int32)ax > (Int32)ay)
            return x;

        if ((Int32)ax == (Int32)ay)
            return (Int32)x < 0? y : x;

        return y;
    }

    public static Int16 MaxMagnitudeNumber(Int16 x, Int16 y) => MaxMagnitude(x, y);

    public static Int16 MinMagnitude(Int16 x, Int16 y)
    {
        Int16 ax = x, ay = y;

        if ((Int32)x < 0)
        {
            ax = (Int16)(-(Int32)x);
            if ((Int32)ax < 0)
                return y;
        }

        if ((Int32)y < 0)
        {
            ay = (Int16)(-(Int32)y);
            if ((Int32)ay < 0)
                return x;
        }

        if ((Int32)ax < (Int32)ay)
            return x;

        if ((Int32)ax == (Int32)ay)
            return (Int32)x < 0? x : y;

        return y;
    }

    public static Int16 MinMagnitudeNumber(Int16 x, Int16 y) => MinMagnitude(x, y);

    public static Int32 Sign(Int16 value) => (Int32)value < 0? -1 : ((Int32)value > 0? 1 : 0);

    public static Boolean IsPow2(Int16 value) => (Int32)value > 0 && (Int32)value != 0 && ((Int32)value & ((Int32)value - 1)) == 0;

    public static Int16 Log2(Int16 value)
    {
        if ((Int32)value < 0)
            throw new OverflowException();

        return (Int16)(16 - 1 - (Int32)LeadingZeroCount((Int16)((Int32)value | 1)));
    }

    public static Tuple<Int16, Int16> DivRem(Int16 left, Int16 right) =>
        new Tuple<Int16, Int16>((Int16)((Int32)left / (Int32)right), (Int16)((Int32)left % (Int32)right));

    public static Int16 LeadingZeroCount(Int16 value)
    {
        UInt32 bits = (UInt32)(UInt16)value;
        Int32 count = 0;

        for (Int32 bit = 16 - 1; bit >= 0; bit--)
        {
            if ((UInt32)((UInt32)(bits >> bit) & (UInt32)1) != (UInt32)0)
                break;

            count++;
        }

        return (Int16)count;
    }

    public static Int16 PopCount(Int16 value)
    {
        UInt32 bits = (UInt32)(UInt16)value;
        Int32 count = 0;

        while (bits != (UInt32)0)
        {
            bits = (UInt32)(bits & (UInt32)(bits - (UInt32)1));
            count++;
        }

        return (Int16)count;
    }

    public static Int16 TrailingZeroCount(Int16 value)
    {
        UInt32 bits = (UInt32)(UInt16)value;
        if (bits == (UInt32)0)
            return (Int16)16;

        Int32 count = 0;
        while ((UInt32)((UInt32)(bits >> count) & (UInt32)1) == (UInt32)0)
            count++;

        return (Int16)count;
    }

    public static Int16 RotateLeft(Int16 value, Int32 rotateAmount)
    {
        Int32 n = rotateAmount & 15;
        if (n == 0)
            return value;

        UInt32 bits = (UInt32)(UInt16)value;
        return (Int16)((UInt32)(bits << n) | (UInt32)(bits >> (16 - n)));
    }

    public static Int16 RotateRight(Int16 value, Int32 rotateAmount)
    {
        Int32 n = rotateAmount & 15;
        if (n == 0)
            return value;

        UInt32 bits = (UInt32)(UInt16)value;
        return (Int16)((UInt32)(bits >> n) | (UInt32)(bits << (16 - n)));
    }

    public Int32 GetByteCount() => 2;

    public Int32 GetShortestBitLength()
    {
        Int16 value = this;

        if ((Int32)value >= 0)
            return 16 - (Int32)LeadingZeroCount(value);

        return 17 - (Int32)LeadingZeroCount((Int16)(~(Int32)value));
    }

    public static Boolean TryConvertFromChecked<TOther>(TOther value, out Int16 result)
        where TOther : Numerics.INumberBase<TOther>
    {
        Boolean negative;
        UInt64 magnitude;

        if (!Numerics.IntegerConversions.TryRead(value, out negative, out magnitude))
        {
            result = (Int16)0;
            return false;
        }

        if (!Numerics.IntegerConversions.Fits(negative, magnitude, 16, true))
            throw new OverflowException();

        UInt64 bits = Numerics.IntegerConversions.Truncate(negative, magnitude);

        result = (Int16)(Int64)bits;
        return true;
    }

    public static Boolean TryConvertToChecked<TTarget>(Int16 value, out TTarget result)
        where TTarget : Numerics.INumberBase<TTarget> => TTarget.TryConvertFromChecked<Int16>(value, out result);

    public static Boolean TryConvertFromSaturating<TOther>(TOther value, out Int16 result)
        where TOther : Numerics.INumberBase<TOther>
    {
        Boolean negative;
        UInt64 magnitude;

        if (!Numerics.IntegerConversions.TryRead(value, out negative, out magnitude))
        {
            result = (Int16)0;
            return false;
        }

        UInt64 bits = Numerics.IntegerConversions.Saturate(negative, magnitude, 16, true);

        result = (Int16)(Int64)bits;
        return true;
    }

    public static Boolean TryConvertToSaturating<TTarget>(Int16 value, out TTarget result)
        where TTarget : Numerics.INumberBase<TTarget> => TTarget.TryConvertFromSaturating<Int16>(value, out result);

    public static Boolean TryConvertFromTruncating<TOther>(TOther value, out Int16 result)
        where TOther : Numerics.INumberBase<TOther>
    {
        Boolean negative;
        UInt64 magnitude;

        if (!Numerics.IntegerConversions.TryRead(value, out negative, out magnitude))
        {
            result = (Int16)0;
            return false;
        }

        UInt64 bits = Numerics.IntegerConversions.Truncate(negative, magnitude);

        result = (Int16)(Int64)bits;
        return true;
    }

    public static Boolean TryConvertToTruncating<TTarget>(Int16 value, out TTarget result)
        where TTarget : Numerics.INumberBase<TTarget> => TTarget.TryConvertFromTruncating<Int16>(value, out result);

    // ---- end of generic math ----
}
