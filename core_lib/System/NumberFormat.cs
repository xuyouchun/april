/*
 *  System.dll
 *
 *  NumberFormat: the text of a number by a format (the numbers' ToString(String, IFormatProvider)).
 */

using Globalization;
using Text;

////////// ////////// ////////// ////////// //////////

// The formats of the numbers (see docs/string-format.md).
//
// A standard format is a letter and the number of digits (the precision): D5, X, F2, N0.
//   integers:  D (decimal, the precision is the least number of digits), X x (hexadecimal; a negative number is the bits of
//              its type), B b (binary), F f, N n (grouped), P p (percent), C c (currency), G g (the default text)
//   floating:  F f, N n, P p, C c, E e (1.234560E+003), G g R r (the default text)
// A format that is not a standard one is a custom format: 0 # . , % 'text' \c ; (see __Custom).
// A format that is not valid is a FormatException.
internal static class NumberFormat
{
    // The text of a signed integer of the number of bits.
    public static String FormatSigned(Int64 value, Int32 bits, String? format, NumberFormatInfo info)
    {
        Boolean negative = value < 0;
        UInt64 magnitude = negative? (UInt64)(-(value + 1)) + 1 : (UInt64)value;

        return FormatInteger(negative, magnitude, bits, format, info);
    }

    // The text of an unsigned integer.
    public static String FormatUnsigned(UInt64 value, Int32 bits, String? format, NumberFormatInfo info)
        => FormatInteger(false, value, bits, format, info);

    private static String FormatInteger(Boolean negative, UInt64 magnitude, Int32 bits, String? format, NumberFormatInfo info)
    {
        if (format == null || format.Length == 0)
            return (negative? info.NegativeSign : "") + magnitude.ToString();

        if (!__IsStandard(format))
            return __Custom(format, negative, true, magnitude, 0.0, info);

        Char letter = format[0];
        Char lower = Char.ToLower(letter);
        Int32 precision = __PrecisionOf(format);

        if (lower == 'd' || lower == 'g')
            return (negative? info.NegativeSign : "") + __PadLeft(magnitude.ToString(), precision, '0');

        if (lower == 'x')
            return __PadLeft(__Radix(__TwosComplement(negative, magnitude, bits), 16, letter == 'X'), precision, '0');

        if (lower == 'b')
            return __PadLeft(__Radix(__TwosComplement(negative, magnitude, bits), 2, false), precision, '0');

        if (lower == 'f' || lower == 'n' || lower == 'p' || lower == 'c')
        {
            String digits = magnitude.ToString();

            // The percent is 100 times the number.
            if (lower == 'p' && magnitude != 0)
                digits = digits + "00";

            Int32 decimals = precision >= 0? precision : __DefaultDigits(lower, info);
            String text = decimals == 0? digits : digits + "." + "".PadRight(decimals, '0');

            return __Compose(lower, negative, text, info);
        }

        throw new FormatException("the format '" + format + "' is not valid for an integer");
    }

    // The text of a floating point number; defaultText is its text when there is no format (or the one of G).
    public static String FormatFloating(Double value, String defaultText, String? format, NumberFormatInfo info)
    {
        if (format == null || format.Length == 0)
            return __Localize(defaultText, info);

        if (value != value)
            return info.NaNSymbol;

        if (value - value != 0)
            return value > 0? info.PositiveInfinitySymbol : info.NegativeInfinitySymbol;

        Boolean negative = value < 0 || (value == 0 && 1.0 / value < 0);
        Double magnitude = negative? -value : value;

        if (!__IsStandard(format))
            return __Custom(format, negative, false, (UInt64)0, magnitude, info);

        Char letter = format[0];
        Char lower = Char.ToLower(letter);
        Int32 precision = __PrecisionOf(format);

        if (lower == 'g' || lower == 'r')
            return __Localize(defaultText, info);

        if (lower == 'f' || lower == 'n' || lower == 'p' || lower == 'c')
        {
            Int32 decimals = precision >= 0? precision : __DefaultDigits(lower, info);
            Double scaled = lower == 'p'? magnitude * 100.0 : magnitude;

            return __Compose(lower, negative, Internal.Double_ToFixed(scaled, decimals), info);
        }

        if (lower == 'e')
        {
            String text = Internal.Double_ToScientific(magnitude, precision < 0? 6 : precision);
            Int32 e = text.IndexOf('e');

            // 1.234560e+03: the exponent has a sign and at least 3 digits.
            String mantissa = text.Substring(0, e).Replace(".", info.NumberDecimalSeparator);
            String sign = text.Substring(e + 1, 1);
            String digits = __PadLeft(text.Substring(e + 2), 3, '0');

            return (negative? info.NegativeSign : "") + mantissa + (letter == 'E'? "E" : "e") + sign + digits;
        }

        throw new FormatException("the format '" + format + "' is not valid for a floating point number");
    }

    // ---- the standard formats

    // Whether the format is a letter and digits (F2), else it is a custom format.
    private static Boolean __IsStandard(String format)
    {
        Char first = format[0];
        if (!((first >= 'A' && first <= 'Z') || (first >= 'a' && first <= 'z')))
            return false;

        for (Int32 k = 1; k < format.Length; k++)
        {
            if (format[k] < '0' || format[k] > '9')
                return false;
        }

        return true;
    }

    // The number of the digits that follows the letter, or -1 when there is none.
    private static Int32 __PrecisionOf(String format)
    {
        Int32 precision = 0;

        for (Int32 k = 1; k < format.Length; k++)
        {
            precision = precision * 10 + ((Int32)format[k] - (Int32)'0');
            if (precision > 99)
                throw new FormatException("the precision of the format '" + format + "' is too large (99)");
        }

        return format.Length == 1? -1 : precision;
    }

    // The digits after the point when the format has none.
    private static Int32 __DefaultDigits(Char lower, NumberFormatInfo info)
    {
        if (lower == 'p')
            return info.PercentDecimalDigits;

        if (lower == 'c')
            return info.CurrencyDecimalDigits;

        return info.NumberDecimalDigits;
    }

    // The text of F, N, P and C: text is the digits of the number (without a sign) with the point, 1234.50.
    private static String __Compose(Char lower, Boolean negative, String text, NumberFormatInfo info)
    {
        Int32 point = text.IndexOf('.');
        String integer = point < 0? text : text.Substring(0, point);
        String fraction = point < 0? "" : info.NumberDecimalSeparator + text.Substring(point + 1);

        if (lower == 'f')
            return (negative? info.NegativeSign : "") + integer + fraction;

        String number = __Group(integer, info.NumberGroupSeparator) + fraction;

        if (lower == 'p')
            return (negative? info.NegativeSign : "") + number + info.PercentSymbol;

        if (lower == 'c')
            return (negative? info.NegativeSign : "") + info.CurrencySymbol + number;

        return (negative? info.NegativeSign : "") + number;
    }

    // The default text of a number with the symbols of the info.
    private static String __Localize(String text, NumberFormatInfo info)
    {
        if (text == "NaN")
            return info.NaNSymbol;

        if (text == "Infinity")
            return info.PositiveInfinitySymbol;

        if (text == "-Infinity")
            return info.NegativeInfinitySymbol;

        if (info.NegativeSign != "-" && text.StartsWith('-'))
            text = info.NegativeSign + text.Substring(1);

        if (info.NumberDecimalSeparator != ".")
            text = text.Replace(".", info.NumberDecimalSeparator);

        return text;
    }

    // The bits of a number of the type: a negative number is 2 ^ bits - the magnitude.
    private static UInt64 __TwosComplement(Boolean negative, UInt64 magnitude, Int32 bits)
    {
        if (!negative)
            return magnitude;

        UInt64 value = ~magnitude + 1;
        return bits >= 64? value : value & (((UInt64)1 << bits) - 1);
    }

    private static String __Radix(UInt64 value, Int32 radix, Boolean upper)
    {
        if (value == 0)
            return "0";

        String digits = upper? "0123456789ABCDEF" : "0123456789abcdef";
        StringBuilder text = new StringBuilder();

        while (value != 0)
        {
            text.Insert(0, digits[(Int32)(value % (UInt64)radix)]);
            value /= (UInt64)radix;
        }

        return text.ToString();
    }

    private static String __PadLeft(String text, Int32 length, Char c) => length > text.Length? text.PadLeft(length, c) : text;

    // The digits without the 0 that are before them ("" for 0).
    private static String __Significant(String digits)
    {
        Int32 start = 0;
        while (start < digits.Length && digits[start] == '0')
            start++;

        return digits.Substring(start);
    }

    // Groups the digits by 3, from the right.
    private static String __Group(String digits, String separator)
    {
        StringBuilder text = new StringBuilder(digits.Length + digits.Length / 3);

        for (Int32 k = 0; k < digits.Length; k++)
        {
            if (k > 0 && (digits.Length - k) % 3 == 0)
                text.Append(separator);

            text.Append(digits[k]);
        }

        return text.ToString();
    }

    // ---- the custom formats

    // A custom format has sections that are split by ; (the first is for the numbers that have no section of their own, the
    // second for the negative numbers, the third for zero). A section is:
    //
    //   0        a digit, or 0                    #        a digit, or nothing
    //   .        the point of the decimals        ,        between two 0 or #: the integer part is grouped by 3
    //   %        the number is 100 times, and the percent symbol is written
    //   'text' "text" \c    the characters as they are       the other characters: as they are
    //
    // The scaling commas (1,000 -> 1) and the exponent (0.0E+00) are not supported (FormatException). A negative number
    // that has no section of its own has the negative sign before the text.
    private static String __Custom(String format, Boolean negative, Boolean isInteger, UInt64 magnitude, Double value,
                                   NumberFormatInfo info)
    {
        Boolean isZero = isInteger? magnitude == 0 : value == 0;

        // The sections.
        Int32 sectionCount = 1;
        Int32[] starts = new Int32[3];
        Int32[] ends = new Int32[3];
        starts[0] = 0;
        ends[0] = format.Length;

        Char quote = '\0';
        for (Int32 k = 0; k < format.Length; k++)
        {
            Char c = format[k];

            if (quote != '\0')
            {
                if (c == quote)
                    quote = '\0';
            }
            else if (c == '\'' || c == '"')
            {
                quote = c;
            }
            else if (c == '\\')
            {
                k++;
            }
            else if (c == ';' && sectionCount < 3)
            {
                ends[sectionCount - 1] = k;
                starts[sectionCount] = k + 1;
                ends[sectionCount] = format.Length;
                sectionCount++;
            }
        }

        Int32 section = 0;
        Boolean sign = negative;

        if (isZero && sectionCount >= 3)
        {
            section = 2;
            sign = false;
        }
        else if (negative && sectionCount >= 2)
        {
            section = 1;
            sign = false;
        }

        String pattern = format.Substring(starts[section], ends[section] - starts[section]);

        // The tokens of the section: 0 characters as they are (text), 1 a digit of the integer part, 2 a digit of the
        // decimals, 3 the point, 4 the percent symbol.
        Int32 length = pattern.Length;
        Int32[] kinds = new Int32[length + 1];
        Char[] chars = new Char[length + 1];
        String[] texts = new String[length + 1];
        Int32 count = 0, integerCount = 0, fractionCount = 0, percents = 0;
        Boolean group = false, point = false;

        for (Int32 k = 0; k < length; k++)
        {
            Char c = pattern[k];

            if (c == '0' || c == '#')
            {
                kinds[count] = point? 2 : 1;
                chars[count++] = c;

                if (point)
                    fractionCount++;
                else
                    integerCount++;
            }
            else if (c == '.' && !point)
            {
                point = true;
                kinds[count++] = 3;
            }
            else if (c == ',' && !point && integerCount > 0)
            {
                // Between two digits: the groups. At the end of the integer digits: scaling (not supported).
                Boolean more = false;
                for (Int32 j = k + 1; j < length && pattern[j] != '.'; j++)
                {
                    if (pattern[j] == '0' || pattern[j] == '#')
                        more = true;
                }

                if (!more)
                    throw new FormatException("the format '" + format + "' scales the number by a comma: it is not supported");

                group = true;
            }
            else if (c == '%')
            {
                percents++;
                kinds[count++] = 4;
            }
            else if (c == '\'' || c == '"')
            {
                Int32 close = pattern.IndexOf(c, k + 1);
                if (close < 0)
                    throw new FormatException("the format '" + format + "' has a quote that is not closed");

                kinds[count] = 0;
                texts[count++] = pattern.Substring(k + 1, close - k - 1);
                k = close;
            }
            else if (c == '\\')
            {
                if (k + 1 >= length)
                    throw new FormatException("the format '" + format + "' ends with a backslash");

                k++;
                kinds[count] = 0;
                texts[count++] = pattern.Substring(k, 1);
            }
            else if ((c == 'E' || c == 'e') && k + 1 < length
                        && (pattern[k + 1] == '0' || pattern[k + 1] == '+' || pattern[k + 1] == '-'))
            {
                throw new FormatException("the format '" + format + "' has an exponent: it is not supported");
            }
            else
            {
                kinds[count] = 0;
                texts[count++] = pattern.Substring(k, 1);
            }
        }

        // The digits of the number: the integer part and the decimals.
        String integer, fraction;

        if (isInteger)
        {
            integer = magnitude.ToString();
            if (percents > 0 && magnitude != 0)
                integer = integer + "".PadRight(percents * 2, '0');

            fraction = "".PadRight(fractionCount, '0');
        }
        else
        {
            Double scaled = value;
            for (Int32 k = 0; k < percents; k++)
                scaled = scaled * 100.0;

            String text = Internal.Double_ToFixed(scaled, fractionCount);
            Int32 dot = text.IndexOf('.');

            integer = dot < 0? text : text.Substring(0, dot);
            fraction = dot < 0? "" : text.Substring(dot + 1);
        }

        String significant = __Significant(integer);

        // The first 0 of the integer digits: the places that are right of it are written (as 0) when the number has no
        // digit there.
        Int32 firstZero = -1, placeholder = 0;
        for (Int32 k = 0; k < count; k++)
        {
            if (kinds[k] == 1)
            {
                if (chars[k] == '0' && firstZero < 0)
                    firstZero = placeholder;

                placeholder++;
            }
        }

        // The decimals that are written: up to the last that is 0 in the format or is not 0 in the number.
        Int32 last = -1, decimalIndex = 0;
        for (Int32 k = 0; k < count; k++)
        {
            if (kinds[k] == 2)
            {
                if (chars[k] == '0' || fraction[decimalIndex] != '0')
                    last = decimalIndex;

                decimalIndex++;
            }
        }

        StringBuilder result = new StringBuilder();
        if (sign)
            result.Append(info.NegativeSign);

        Int32 integerIndex = 0, fractionIndex = 0;

        for (Int32 k = 0; k < count; k++)
        {
            Int32 kind = kinds[k];

            if (kind == 0)
            {
                result.Append(texts[k]);
            }
            else if (kind == 4)
            {
                result.Append(info.PercentSymbol);
            }
            else if (kind == 3)
            {
                if (last >= 0)
                    result.Append(info.NumberDecimalSeparator);
            }
            else if (kind == 1)
            {
                // The place of the digit from the right: the first placeholder writes the digits that are before it too.
                Int32 place = integerCount - 1 - integerIndex;
                Int32 high = place;
                if (integerIndex == 0 && significant.Length - 1 > place)
                    high = significant.Length - 1;

                for (Int32 j = high; j >= place; j--)
                {
                    Boolean hasDigit = j < significant.Length;
                    Boolean padded = firstZero >= 0 && integerIndex >= firstZero;

                    if (hasDigit || padded)
                    {
                        result.Append(hasDigit? significant[significant.Length - 1 - j] : '0');

                        if (group && j > 0 && j % 3 == 0)
                            result.Append(info.NumberGroupSeparator);
                    }
                }

                integerIndex++;
            }
            else
            {
                if (fractionIndex <= last)
                    result.Append(fraction[fractionIndex]);

                fractionIndex++;
            }
        }

        return result.ToString();
    }
}
