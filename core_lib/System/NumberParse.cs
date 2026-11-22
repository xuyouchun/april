/*
 *  System.dll
 *
 *  NumberParse: the number of a text (the Parse and TryParse of the numbers).
 */

using Globalization;

////////// ////////// ////////// ////////// //////////

// The text of a number: spaces, a sign, the digits (integers); floating numbers also have a decimal separator, an exponent (e or E),
// NaN and the infinities (the symbols of the NumberFormatInfo). Hexadecimal numbers, thousands separators, currency and parentheses are
// not accepted. Parse throws FormatException for a text that is not a number, OverflowException for a number that does not fit.
internal static class NumberParse
{
    private const Int32 __ok = 0;
    private const Int32 __format = 1;
    private const Int32 __overflow = 2;

    // The sign at the beginning of the text (the index goes after it).
    private static Boolean __Sign(String text, NumberFormatInfo info, ref Int32 index)
    {
        String negative = info.NegativeSign;

        if (negative.Length > 0 && __StartsAt(text, index, negative))
        {
            index = index + negative.Length;
            return true;
        }

        if (__StartsAt(text, index, "+"))
            index = index + 1;

        return false;
    }

    private static Boolean __StartsAt(String text, Int32 index, String part)
    {
        if (part.Length == 0 || index + part.Length > text.Length)
            return false;

        for (Int32 k = 0; k < part.Length; k++)
        {
            if (text[index + k] != part[k])
                return false;
        }

        return true;
    }

    // The digits of an integer: the magnitude and the sign. The status is __ok, __format, or __overflow (more than 64 bits).
    private static Int32 __Integer(String? text, NumberFormatInfo info, out Boolean negative, out UInt64 magnitude)
    {
        negative = false;
        magnitude = 0;

        if (text == null)
            return __format;

        String trimmed = text.Trim();
        Int32 index = 0;

        negative = __Sign(trimmed, info, ref index);
        if (index >= trimmed.Length)
            return __format;

        UInt64 value = 0;
        Boolean overflow = false;

        for (; index < trimmed.Length; index++)
        {
            Int32 digit = (Int32)trimmed[index] - 48;
            if (digit < 0 || digit > 9)
                return __format;

            // value * 10 + digit must fit in 64 bits.
            if (value > 1844674407370955161 || (value == 1844674407370955161 && digit > 5))
                overflow = true;
            else
                value = value * 10 + (UInt64)digit;
        }

        magnitude = value;
        return overflow ? __overflow : __ok;
    }

    // A signed integer between min and max; the status is as __Integer, the number is in value.
    internal static Int32 Signed(String? text, NumberFormatInfo info, Int64 min, Int64 max, out Int64 value)
    {
        Boolean negative;
        UInt64 magnitude;

        value = 0;

        Int32 status = __Integer(text, info, out negative, out magnitude);
        if (status != __ok && status != __overflow)
            return status;

        if (status == __overflow)
            return __overflow;

        if (negative)
        {
            // The magnitude of the least number is one more than the greatest of its magnitudes.
            UInt64 limit = (UInt64)(-(min + 1)) + 1;
            if (magnitude > limit)
                return __overflow;

            value = magnitude == limit ? min : -(Int64)magnitude;
            return __ok;
        }

        if (magnitude > (UInt64)max)
            return __overflow;

        value = (Int64)magnitude;
        return __ok;
    }

    internal static Int32 Unsigned(String? text, NumberFormatInfo info, UInt64 max, out UInt64 value)
    {
        Boolean negative;
        UInt64 magnitude;

        value = 0;

        Int32 status = __Integer(text, info, out negative, out magnitude);
        if (status != __ok)
            return status;

        // "-0" is zero, any other negative number does not fit.
        if ((negative && magnitude != 0) || magnitude > max)
            return __overflow;

        value = magnitude;
        return __ok;
    }

    // The exceptions of Parse.
    internal static Void Throw(Int32 status, String? text, String type)
    {
        if (text == null)
            throw new ArgumentException("the text is null");

        if (status == __overflow)
            throw new OverflowException("the value '" + text + "' does not fit in " + type);

        throw new FormatException("the text '" + text + "' is not a valid " + type);
    }

    ////////// ////////// ////////// ////////// //////////

    // A floating number: 0 the text is not a number, 1 a number (the ASCII form is in normalized: [-]digits.digits[e[-]digits]),
    // 2 NaN, 3 +infinity, 4 -infinity.
    internal static Int32 Floating(String? text, NumberFormatInfo info, out String normalized)
    {
        normalized = "";

        if (text == null)
            return 0;

        String trimmed = text.Trim();
        if (trimmed.Length == 0)
            return 0;

        // The names of the special values (their case is not important).
        String lower = trimmed.ToLower();
        if (lower == info.NaNSymbol.ToLower())
            return 2;

        if (lower == info.PositiveInfinitySymbol.ToLower() || lower == "+" + info.PositiveInfinitySymbol.ToLower())
            return 3;

        if (lower == info.NegativeInfinitySymbol.ToLower())
            return 4;

        Int32 index = 0;
        Boolean negative = __Sign(trimmed, info, ref index);

        String whole = "";
        String fraction = "";

        while (index < trimmed.Length && trimmed[index] >= '0' && trimmed[index] <= '9')
        {
            whole = whole + trimmed[index];
            index = index + 1;
        }

        String separator = info.NumberDecimalSeparator;
        if (separator.Length > 0 && __StartsAt(trimmed, index, separator))
        {
            index = index + separator.Length;

            while (index < trimmed.Length && trimmed[index] >= '0' && trimmed[index] <= '9')
            {
                fraction = fraction + trimmed[index];
                index = index + 1;
            }
        }

        if (whole.Length == 0 && fraction.Length == 0)
            return 0;

        String exponent = "";

        if (index < trimmed.Length && (trimmed[index] == 'e' || trimmed[index] == 'E'))
        {
            index = index + 1;

            String sign = "";
            if (index < trimmed.Length && (trimmed[index] == '+' || trimmed[index] == '-'))
            {
                sign = trimmed[index] == '-' ? "-" : "";
                index = index + 1;
            }

            String digits = "";
            while (index < trimmed.Length && trimmed[index] >= '0' && trimmed[index] <= '9')
            {
                digits = digits + trimmed[index];
                index = index + 1;
            }

            if (digits.Length == 0)
                return 0;

            exponent = "e" + sign + digits;
        }

        if (index != trimmed.Length)
            return 0;

        normalized = (negative ? "-" : "") + (whole.Length == 0 ? "0" : whole) + "." + (fraction.Length == 0 ? "0" : fraction) + exponent;
        return 1;
    }

    internal static Double NaNValue()
    {
        Double zero = 0;
        return zero / zero;
    }

    internal static Double InfinityValue()
    {
        Double zero = 0;
        return 1 / zero;
    }
}
