
/*
 *  System.dll
 *
 *  Char: the members of the characters (the declaration is in +types.cs).
 */

////////// ////////// ////////// ////////// //////////

public partial struct Char
{
    public Int32 CompareTo(Char other) => Primitives.Compare(this, other);

    public Boolean Equals(Char other) => Primitives.Same(this, other);

    public Boolean Equals(Object? obj) => obj is Char && Primitives.Same(this, (Char)obj);

    public Int32 GetHashCode() => Primitives.Hash(this);

    public String ToString() => Internal.Char_ToString(this);

    public static Boolean IsLetter(Char c) => Internal.Char_IsLetter(c);

    public static Boolean IsDigit(Char c) => Internal.Char_IsDigit(c);

    public static Boolean IsLetterOrDigit(Char c) => Internal.Char_IsLetter(c) || Internal.Char_IsDigit(c);

    public static Boolean IsWhiteSpace(Char c) => Internal.Char_IsWhiteSpace(c);

    public static Boolean IsUpper(Char c) => Internal.Char_ToLower(c) != c;

    public static Boolean IsLower(Char c) => Internal.Char_ToUpper(c) != c;

    // The general category of Unicode (Lu, Nd, Zs ...), from the table of CharUnicodeInfo.
    public static Globalization.UnicodeCategory GetUnicodeCategory(Char c) => Globalization.CharUnicodeInfo.GetUnicodeCategory(c);

    public static Char ToUpper(Char c) => Internal.Char_ToUpper(c);

    public static Char ToLower(Char c) => Internal.Char_ToLower(c);

    // The two halves of a character that is not in the basic plane (UTF-16).
    public static Boolean IsHighSurrogate(Char c) => c >= '\uD800' && c <= '\uDBFF';

    public static Boolean IsLowSurrogate(Char c) => c >= '\uDC00' && c <= '\uDFFF';

    public static Boolean IsSurrogate(Char c) => c >= '\uD800' && c <= '\uDFFF';
};
