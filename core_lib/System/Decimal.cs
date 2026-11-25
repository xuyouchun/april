/*
 *  System.dll
 *
 *  Decimal: the numbers that have a decimal point and are exact (1.5m, money). A decimal is a mantissa of 64 bits and a scale: the
 *  value is mantissa / 10 ^ scale, the scale is from 0 to 18 (1.50m has the scale 2, it is written as it is). The operations are made
 *  in 128 bits, then the result is brought back to the mantissa: the digits that do not fit are rounded (the last digit that is
 *  lost is rounded half up), OverflowException when the integer part does not fit.
 *
 *  The literal 1.5m is Decimal.Parse("1.5") (the compiler writes the call).
 */

////////// ////////// ////////// ////////// //////////

public struct Decimal : IStringable, IComparable<Decimal>, IEquatable<Decimal>
{
    private Int64 __mantissa;
    private Int32 __scale;

    private Decimal(Int64 mantissa, Int32 scale)
    {
        __mantissa = mantissa;
        __scale = scale;
    }

    ////////// ////////// ////////// ////////// //////////
    // The constants.

    public static Decimal Zero => new Decimal(0, 0);

    public static Decimal One => new Decimal(1, 0);

    public static Decimal MinusOne => new Decimal(-1, 0);

    public static Decimal MaxValue => new Decimal(9223372036854775807, 0);

    public static Decimal MinValue => new Decimal(-9223372036854775807, 0);

    public Int32 Scale => __scale;

    public Boolean IsZero => __mantissa == 0;

    public Int32 Sign => __mantissa > 0? 1 : (__mantissa < 0? -1 : 0);

    ////////// ////////// ////////// ////////// //////////
    // 128 bits: the high and the low 64 bits of a number that has no sign.

    // 10 ^ n, n is from 0 to 19.
    private static UInt64 __Pow10(Int32 n)
    {
        UInt64 value = 1;

        for (Int32 k = 0; k < n; k++)
            value = value * 10;

        return value;
    }

    // The product of two numbers of 64 bits.
    private static Void __Mul(UInt64 a, UInt64 b, out UInt64 hi, out UInt64 lo)
    {
        UInt64 mask = 4294967295;

        UInt64 a0 = a & mask, a1 = a >> 32;
        UInt64 b0 = b & mask, b1 = b >> 32;

        UInt64 p00 = a0 * b0;
        UInt64 p01 = a0 * b1;
        UInt64 p10 = a1 * b0;
        UInt64 p11 = a1 * b1;

        UInt64 middle = (p00 >> 32) + (p01 & mask) + (p10 & mask);

        lo = (p00 & mask) | (middle << 32);
        hi = p11 + (p01 >> 32) + (p10 >> 32) + (middle >> 32);
    }

    // The 128 bits number times 10: false when it does not fit.
    private static Boolean __Mul10(ref UInt64 hi, ref UInt64 lo)
    {
        UInt64 lo_hi, lo_lo;
        __Mul(lo, (UInt64)10, out lo_hi, out lo_lo);

        UInt64 hi_hi, hi_lo;
        __Mul(hi, (UInt64)10, out hi_hi, out hi_lo);

        if (hi_hi != 0)
            return false;

        UInt64 sum = hi_lo + lo_hi;
        if (sum < hi_lo)
            return false;

        hi = sum;
        lo = lo_lo;
        return true;
    }

    // The 128 bits number divided by a number of 64 bits: the quotient and the remainder.
    private static UInt64 __Divide(UInt64 hi, UInt64 lo, UInt64 divisor, out UInt64 quotient_hi, out UInt64 quotient_lo)
    {
        UInt64 remainder = 0;
        UInt64 q_hi = 0, q_lo = 0;

        for (Int32 i = 127; i >= 0; i--)
        {
            UInt64 bit = i >= 64? (hi >> (i - 64)) & 1 : (lo >> i) & 1;

            UInt64 carry = remainder >> 63;
            remainder = (remainder << 1) | bit;

            if (carry != 0 || remainder >= divisor)
            {
                remainder = remainder - divisor;

                if (i >= 64)
                    q_hi = q_hi | ((UInt64)1 << (i - 64));
                else
                    q_lo = q_lo | ((UInt64)1 << i);
            }
        }

        quotient_hi = q_hi;
        quotient_lo = q_lo;
        return remainder;
    }

    // The magnitude of a number, as 128 bits (the number is multiplied by 10 ^ shift).
    private static Void __Magnitude(Int64 mantissa, Int32 shift, out UInt64 hi, out UInt64 lo)
    {
        UInt64 value = mantissa < 0? (UInt64)(-mantissa) : (UInt64)mantissa;

        hi = 0;
        lo = value;

        for (Int32 k = 0; k < shift; k++)
        {
            if (!__Mul10(ref hi, ref lo))
                throw new OverflowException();
        }
    }

    // The decimal that a magnitude of 128 bits and a scale are: the digits that do not fit are cut, and the last one that is cut
    // rounds the number (half up).
    private static Decimal __Make(Boolean negative, UInt64 hi, UInt64 lo, Int32 scale)
    {
        UInt64 last = 0;
        Boolean cut = false;

        // The scale is from 0 to 18, the mantissa has 63 bits.
        while (scale > 18 || hi != 0 || lo > 9223372036854775807)
        {
            if (scale <= 0)
                throw new OverflowException();

            UInt64 q_hi, q_lo;
            last = __Divide(hi, lo, (UInt64)10, out q_hi, out q_lo);

            hi = q_hi;
            lo = q_lo;
            scale--;
            cut = true;
        }

        if (cut && last >= 5)
        {
            lo = lo + 1;

            if (lo > 9223372036854775807)
            {
                if (scale <= 0)
                    throw new OverflowException();

                lo = lo / 10;
                scale--;
            }
        }

        Int64 mantissa = (Int64)lo;
        return new Decimal(negative? -mantissa : mantissa, scale);
    }

    ////////// ////////// ////////// ////////// //////////
    // The operations.

    public static Decimal operator +(Decimal left, Decimal right)
    {
        Int32 scale = left.__scale > right.__scale? left.__scale : right.__scale;

        UInt64 a_hi, a_lo, b_hi, b_lo;
        __Magnitude(left.__mantissa, scale - left.__scale, out a_hi, out a_lo);
        __Magnitude(right.__mantissa, scale - right.__scale, out b_hi, out b_lo);

        Boolean a_negative = left.__mantissa < 0, b_negative = right.__mantissa < 0;

        UInt64 hi, lo;
        Boolean negative;

        if (a_negative == b_negative)
        {
            lo = a_lo + b_lo;
            hi = a_hi + b_hi + (lo < a_lo? (UInt64)1 : (UInt64)0);
            negative = a_negative;
        }
        else
        {
            // The larger magnitude minus the smaller one.
            Boolean a_larger = a_hi > b_hi || (a_hi == b_hi && a_lo >= b_lo);

            if (!a_larger)
            {
                UInt64 t = a_hi; a_hi = b_hi; b_hi = t;
                t = a_lo; a_lo = b_lo; b_lo = t;
            }

            lo = a_lo - b_lo;
            hi = a_hi - b_hi - (a_lo < b_lo? (UInt64)1 : (UInt64)0);
            negative = a_larger? a_negative : b_negative;
        }

        return __Make(negative && (hi != 0 || lo != 0), hi, lo, scale);
    }

    public static Decimal operator -(Decimal value) => new Decimal(-value.__mantissa, value.__scale);

    public static Decimal operator +(Decimal value) => value;

    public static Decimal operator -(Decimal left, Decimal right) => left + (-right);

    public static Decimal operator *(Decimal left, Decimal right)
    {
        UInt64 hi, lo;
        __Mul(left.__mantissa < 0? (UInt64)(-left.__mantissa) : (UInt64)left.__mantissa,
              right.__mantissa < 0? (UInt64)(-right.__mantissa) : (UInt64)right.__mantissa, out hi, out lo);

        Boolean negative = (left.__mantissa < 0) != (right.__mantissa < 0);
        return __Make(negative, hi, lo, left.__scale + right.__scale);
    }

    public static Decimal operator /(Decimal left, Decimal right)
    {
        if (right.__mantissa == 0)
            throw new DivideByZeroException();

        // The mantissa of the dividend times 10 ^ 18: the quotient has 18 digits more than the integers.
        UInt64 hi, lo;
        __Magnitude(left.__mantissa, 18, out hi, out lo);

        UInt64 divisor = right.__mantissa < 0? (UInt64)(-right.__mantissa) : (UInt64)right.__mantissa;

        UInt64 q_hi, q_lo;
        __Divide(hi, lo, divisor, out q_hi, out q_lo);

        Int32 scale = 18 + left.__scale - right.__scale;

        // A negative scale is a number that is multiplied by 10 ^ -scale.
        while (scale < 0)
        {
            if (!__Mul10(ref q_hi, ref q_lo))
                throw new OverflowException();

            scale++;
        }

        Boolean negative = (left.__mantissa < 0) != (right.__mantissa < 0);
        Decimal result = __Make(negative, q_hi, q_lo, scale);

        // The zeros at the end are not kept, down to the scale that the operands tell.
        Int32 preferred = left.__scale > right.__scale? left.__scale - right.__scale : 0;
        Int64 mantissa = result.__mantissa;
        Int32 result_scale = result.__scale;

        while (result_scale > preferred && mantissa % 10 == 0)
        {
            mantissa = mantissa / 10;
            result_scale--;
        }

        return new Decimal(mantissa, result_scale);
    }

    // The remainder: left - right * (the integer part of left / right).
    public static Decimal operator %(Decimal left, Decimal right)
    {
        if (right.__mantissa == 0)
            throw new DivideByZeroException();

        return left - right * Truncate(left / right);
    }

    public static Decimal operator ++(Decimal value) => value + One;

    public static Decimal operator --(Decimal value) => value - One;

    ////////// ////////// ////////// ////////// //////////
    // The comparison.

    public Int32 CompareTo(Decimal other)
    {
        Int32 scale = __scale > other.__scale? __scale : other.__scale;

        UInt64 a_hi, a_lo, b_hi, b_lo;
        __Magnitude(__mantissa, scale - __scale, out a_hi, out a_lo);
        __Magnitude(other.__mantissa, scale - other.__scale, out b_hi, out b_lo);

        Int32 a_sign = Sign, b_sign = other.Sign;
        if (a_sign != b_sign)
            return a_sign < b_sign? -1 : 1;

        Int32 magnitude = 0;
        if (a_hi != b_hi)
            magnitude = a_hi < b_hi? -1 : 1;
        else if (a_lo != b_lo)
            magnitude = a_lo < b_lo? -1 : 1;

        return a_sign < 0? -magnitude : magnitude;
    }

    public Boolean Equals(Decimal other) => CompareTo(other) == 0;

    public Boolean Equals(Object? obj) => obj is Decimal && CompareTo((Decimal)obj) == 0;

    public Int32 GetHashCode()
    {
        // 1.0 and 1.00 are the same value.
        Int64 mantissa = __mantissa;
        Int32 scale = __scale;

        while (scale > 0 && mantissa % 10 == 0)
        {
            mantissa = mantissa / 10;
            scale--;
        }

        return (Int32)(mantissa ^ (mantissa >> 32)) * 31 + scale;
    }

    public static Boolean operator ==(Decimal left, Decimal right) => left.CompareTo(right) == 0;

    public static Boolean operator !=(Decimal left, Decimal right) => left.CompareTo(right) != 0;

    public static Boolean operator <(Decimal left, Decimal right) => left.CompareTo(right) < 0;

    public static Boolean operator <=(Decimal left, Decimal right) => left.CompareTo(right) <= 0;

    public static Boolean operator >(Decimal left, Decimal right) => left.CompareTo(right) > 0;

    public static Boolean operator >=(Decimal left, Decimal right) => left.CompareTo(right) >= 0;

    ////////// ////////// ////////// ////////// //////////
    // The rounding.

    // The integer part: 1.9 is 1, -1.9 is -1.
    public static Decimal Truncate(Decimal value)
    {
        if (value.__scale == 0)
            return value;

        return new Decimal(value.__mantissa / (Int64)__Pow10(value.__scale), 0);
    }

    public static Decimal Floor(Decimal value)
    {
        Decimal truncated = Truncate(value);
        return truncated > value? truncated - One : truncated;
    }

    public static Decimal Ceiling(Decimal value)
    {
        Decimal truncated = Truncate(value);
        return truncated < value? truncated + One : truncated;
    }

    // The value with the digits after the point: the middle is rounded to the even digit (Round(2.5m) is 2, Round(3.5m) is 4).
    public static Decimal Round(Decimal value, Int32 digits)
    {
        if (digits < 0 || digits > 18)
            throw new OutOfRangeException();

        if (value.__scale <= digits)
            return value;

        Int64 divisor = (Int64)__Pow10(value.__scale - digits);
        Int64 quotient = value.__mantissa / divisor;
        Int64 remainder = value.__mantissa % divisor;
        Int64 twice = (remainder < 0? -remainder : remainder) * 2;

        if (twice > divisor || (twice == divisor && quotient % 2 != 0))
            quotient = value.__mantissa < 0? quotient - 1 : quotient + 1;

        return new Decimal(quotient, digits);
    }

    public static Decimal Round(Decimal value) => Round(value, 0);

    public static Decimal Abs(Decimal value) => value.__mantissa < 0? -value : value;

    public static Decimal Min(Decimal left, Decimal right) => left <= right? left : right;

    public static Decimal Max(Decimal left, Decimal right) => left >= right? left : right;

    ////////// ////////// ////////// ////////// //////////
    // The conversions.

    public static implicit operator Decimal(Int8 value) => new Decimal((Int64)value, 0);

    public static implicit operator Decimal(UInt8 value) => new Decimal((Int64)value, 0);

    public static implicit operator Decimal(Int16 value) => new Decimal((Int64)value, 0);

    public static implicit operator Decimal(UInt16 value) => new Decimal((Int64)value, 0);

    public static implicit operator Decimal(Int32 value) => new Decimal((Int64)value, 0);

    public static implicit operator Decimal(UInt32 value) => new Decimal((Int64)value, 0);

    public static implicit operator Decimal(Int64 value)
    {
        if (value == -9223372036854775807 - 1)
            throw new OverflowException();

        return new Decimal(value, 0);
    }

    public static implicit operator Decimal(UInt64 value)
    {
        if (value > 9223372036854775807)
            throw new OverflowException();

        return new Decimal((Int64)value, 0);
    }

    // The text that a number of Double has is its value, as far as the digits of it go.
    public static explicit operator Decimal(Double value) => Parse(value.ToString());

    public static explicit operator Decimal(Single value) => Parse(((Double)value).ToString());

    public static explicit operator Double(Decimal value) => (Double)value.__mantissa / (Double)__Pow10(value.__scale);

    public static explicit operator Single(Decimal value) => (Single)(Double)value;

    public static explicit operator Int64(Decimal value) => Truncate(value).__mantissa;

    public static explicit operator Int32(Decimal value)
    {
        Int64 integer = Truncate(value).__mantissa;
        if (integer < -2147483648 || integer > 2147483647)
            throw new OverflowException();

        return (Int32)integer;
    }

    public static explicit operator UInt64(Decimal value)
    {
        Int64 integer = Truncate(value).__mantissa;
        if (integer < 0)
            throw new OverflowException();

        return (UInt64)integer;
    }

    public static explicit operator UInt32(Decimal value)
    {
        Int64 integer = Truncate(value).__mantissa;
        if (integer < 0 || integer > 4294967295)
            throw new OverflowException();

        return (UInt32)integer;
    }

    ////////// ////////// ////////// ////////// //////////
    // The text.

    // 12.50: the digits, the point at the scale.
    public String ToString()
    {
        Boolean negative = __mantissa < 0;
        UInt64 magnitude = negative? (UInt64)(-(__mantissa + 1)) + 1 : (UInt64)__mantissa;
        String digits = magnitude.ToString();

        if (__scale > 0)
        {
            while (digits.Length <= __scale)
                digits = "0" + digits;

            digits = digits.Substring(0, digits.Length - __scale) + "." + digits.Substring(digits.Length - __scale);
        }

        return negative? "-" + digits : digits;
    }

    public static Decimal Parse(String text)
    {
        Decimal result;
        if (!TryParse(text, out result))
            throw new FormatException("The text is not a decimal number.");

        return result;
    }

    public static Boolean TryParse(String? text, out Decimal result)
    {
        result = Zero;
        if (text == null)
            return false;

        Int32 position = 0, length = text.Length;
        while (position < length && Char.IsWhiteSpace(text[position]))
            position++;

        Boolean negative = false;
        if (position < length && (text[position] == '-' || text[position] == '+'))
        {
            negative = text[position] == '-';
            position++;
        }

        UInt64 hi = 0, lo = 0;
        Int32 scale = 0, digits = 0;
        Boolean point = false;

        while (position < length)
        {
            Char c = text[position];

            if (c == '.' && !point)
            {
                point = true;
            }
            else if (c >= '0' && c <= '9')
            {
                if (!__Mul10(ref hi, ref lo))
                    return false;

                UInt64 digit = (UInt64)(c - '0');
                UInt64 sum = lo + digit;

                if (sum < lo)
                {
                    hi = hi + 1;
                    if (hi == 0)
                        return false;
                }

                lo = sum;
                digits++;

                if (point)
                    scale++;
            }
            else
            {
                break;
            }

            position++;
        }

        if (digits == 0)
            return false;

        // The exponent: 1.5e3.
        Int32 exponent = 0;
        if (position < length && (text[position] == 'e' || text[position] == 'E'))
        {
            position++;

            Boolean exponent_negative = false;
            if (position < length && (text[position] == '-' || text[position] == '+'))
            {
                exponent_negative = text[position] == '-';
                position++;
            }

            Int32 exponent_digits = 0;
            while (position < length && text[position] >= '0' && text[position] <= '9' && exponent < 1000)
            {
                exponent = exponent * 10 + (text[position] - '0');
                exponent_digits++;
                position++;
            }

            if (exponent_digits == 0)
                return false;

            if (exponent_negative)
                exponent = -exponent;
        }

        while (position < length && Char.IsWhiteSpace(text[position]))
            position++;

        if (position != length)
            return false;

        scale = scale - exponent;

        // A negative scale: the number is multiplied by 10 ^ -scale.
        while (scale < 0)
        {
            if (!__Mul10(ref hi, ref lo))
                return false;

            scale++;
        }

        try
        {
            result = __Make(negative && (hi != 0 || lo != 0), hi, lo, scale);
            return true;
        }
        catch (OverflowException)
        {
            return false;
        }
    }
}

////////// ////////// ////////// ////////// //////////

/// <summary>The literal 1.5m: the compiler writes DecimalLiteral.Of("1.5").</summary>
public static class DecimalLiteral
{
    public static Decimal Of(String text) => Decimal.Parse(text);
}
