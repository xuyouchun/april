/*
 *  System.dll
 *
 *  Rune: a Unicode scalar value, a character of any plane.
 */

namespace Text;

////////// ////////// ////////// ////////// //////////

// A Unicode scalar value: U+0000 to U+10FFFF without the surrogates (U+D800 to U+DFFF). A Char is a unit of
// UTF-16, a Rune is a whole character: the two Chars of a pair of surrogates are one Rune.
public struct Rune : IComparable<Rune>, IEquatable<Rune>, IEquality, IStringable
{
    // The value must be a scalar value, else it is an ArgumentException.
    public Rune(Int32 value)
    {
        if (!IsValid(value))
            throw new ArgumentException();

        __value = value;
    }

    // A character that is not a surrogate.
    public Rune(Char value)
    {
        if (Char.IsSurrogate(value))
            throw new ArgumentException();

        __value = (Int32)value;
    }

    // A high and a low surrogate that make a pair.
    public Rune(Char high, Char low)
    {
        if (!Char.IsHighSurrogate(high) || !Char.IsLowSurrogate(low))
            throw new ArgumentException();

        __value = 0x10000 + (((Int32)high - 0xD800) << 10) + ((Int32)low - 0xDC00);
    }

    // The scalar value.
    public Int32 Value => __value;

    // Whether the value is in the basic plane (one Char).
    public Boolean IsBmp => __value < 0x10000;

    // The plane: 0 is the basic plane.
    public Int32 Plane => __value >> 16;

    // The number of Chars (1 or 2) and of bytes in UTF-8 (1 to 4) that the rune has.
    public Int32 Utf16SequenceLength => __value < 0x10000 ? 1 : 2;

    public Int32 Utf8SequenceLength => __value < 0x80 ? 1 : (__value < 0x800 ? 2 : (__value < 0x10000 ? 3 : 4));

    // The replacement character, U+FFFD.
    public static Rune ReplacementChar => new Rune(0xFFFD);

    // Whether the value is a scalar value: not a surrogate, not over U+10FFFF, not negative.
    public static Boolean IsValid(Int32 value) => value >= 0 && value <= 0x10FFFF && (value < 0xD800 || value > 0xDFFF);

    // The rune that starts at the index of the string. A surrogate that has no pair is U+FFFD.
    public static Rune GetRuneAt(String s, Int32 index)
    {
        if (index < 0 || index >= s.Length)
            throw new OutOfRangeException();

        Char c = s[index];
        if (!Char.IsSurrogate(c))
            return new Rune((Int32)c);

        if (Char.IsHighSurrogate(c) && index + 1 < s.Length && Char.IsLowSurrogate(s[index + 1]))
            return new Rune(c, s[index + 1]);

        return ReplacementChar;
    }

    // The number of runes of the string (a pair of surrogates is one, a surrogate that has no pair is one).
    public static Int32 CountRunes(String s)
    {
        Int32 count = 0;
        for (Int32 k = 0; k < s.Length; k++)
        {
            if (Char.IsHighSurrogate(s[k]) && k + 1 < s.Length && Char.IsLowSurrogate(s[k + 1]))
                k++;

            count++;
        }

        return count;
    }

    // The runes of the string.
    public static Rune[] ToRunes(String s)
    {
        Rune[] runes = new Rune[CountRunes(s)];
        Int32 n = 0;

        for (Int32 k = 0; k < s.Length; k++)
        {
            Rune r = GetRuneAt(s, k);
            if (r.Utf16SequenceLength == 2 && r.Value != 0xFFFD)
                k++;

            runes[n++] = r;
        }

        return runes;
    }

    // The properties and the case. They are the ones of Char: a rune that is not in the basic plane is not a
    // letter, a digit, a white space or an upper or lower case letter, and its case is not changed.
    public static Boolean IsLetter(Rune r) => r.__value < 0x10000 && Char.IsLetter((Char)r.__value);

    public static Boolean IsDigit(Rune r) => r.__value < 0x10000 && Char.IsDigit((Char)r.__value);

    public static Boolean IsLetterOrDigit(Rune r) => IsLetter(r) || IsDigit(r);

    public static Boolean IsWhiteSpace(Rune r) => r.__value < 0x10000 && Char.IsWhiteSpace((Char)r.__value);

    public static Boolean IsUpper(Rune r) => r.__value < 0x10000 && Char.IsUpper((Char)r.__value);

    public static Boolean IsLower(Rune r) => r.__value < 0x10000 && Char.IsLower((Char)r.__value);

    // The general category of Unicode of the rune, of any plane.
    public static Globalization.UnicodeCategory GetUnicodeCategory(Rune r) => Globalization.CharUnicodeInfo.GetUnicodeCategory(r.__value);

    public static Rune ToUpper(Rune r) => r.__value < 0x10000 ? new Rune((Int32)Char.ToUpper((Char)r.__value)) : r;

    public static Rune ToLower(Rune r) => r.__value < 0x10000 ? new Rune((Int32)Char.ToLower((Char)r.__value)) : r;

    public Int32 CompareTo(Rune other) => __value < other.__value ? -1 : (__value > other.__value ? 1 : 0);

    public Boolean Equals(Rune other) => __value == other.__value;

    public Boolean Equals(Object? obj) => obj is Rune && __value == ((Rune)obj).__value;

    public Int32 GetHashCode() => __value;

    // The string of one or two Chars.
    public String ToString()
    {
        if (__value < 0x10000)
            return ((Char)__value).ToString();

        Int32 v = __value - 0x10000;
        return String.FromChars(new Char[] { (Char)(0xD800 + (v >> 10)), (Char)(0xDC00 + (v & 0x3FF)) });
    }

    public static Boolean operator ==(Rune left, Rune right) => left.__value == right.__value;

    public static Boolean operator !=(Rune left, Rune right) => left.__value != right.__value;

    public static Boolean operator <(Rune left, Rune right) => left.__value < right.__value;

    public static Boolean operator >(Rune left, Rune right) => left.__value > right.__value;

    public static Boolean operator <=(Rune left, Rune right) => left.__value <= right.__value;

    public static Boolean operator >=(Rune left, Rune right) => left.__value >= right.__value;

    // The conversions check the value, a value that is not a scalar value is an ArgumentException.
    public static explicit operator Rune(Int32 value) => new Rune(value);

    public static explicit operator Rune(Char value) => new Rune(value);

    public static explicit operator Int32(Rune r) => r.__value;

    private readonly Int32 __value;
}
