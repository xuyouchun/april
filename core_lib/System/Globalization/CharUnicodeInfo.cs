/*
 *  System.dll
 *
 *  UnicodeCategory, CharUnicodeInfo: the general category of a character of Unicode (Lu, Nd, Zs ...).
 */

namespace Globalization;

////////// ////////// ////////// ////////// //////////

// The general categories of Unicode, with the numbers of .NET.
public enum UnicodeCategory : Int32
{
    UppercaseLetter = 0,
    LowercaseLetter = 1,
    TitlecaseLetter = 2,
    ModifierLetter = 3,
    OtherLetter = 4,
    NonSpacingMark = 5,
    SpacingCombiningMark = 6,
    EnclosingMark = 7,
    DecimalDigitNumber = 8,
    LetterNumber = 9,
    OtherNumber = 10,
    SpaceSeparator = 11,
    LineSeparator = 12,
    ParagraphSeparator = 13,
    Control = 14,
    Format = 15,
    Surrogate = 16,
    PrivateUse = 17,
    ConnectorPunctuation = 18,
    DashPunctuation = 19,
    OpenPunctuation = 20,
    ClosePunctuation = 21,
    InitialQuotePunctuation = 22,
    FinalQuotePunctuation = 23,
    OtherPunctuation = 24,
    MathSymbol = 25,
    CurrencySymbol = 26,
    ModifierSymbol = 27,
    OtherSymbol = 28,
    OtherNotAssigned = 29;
}

////////// ////////// ////////// ////////// //////////

// The general category of a character. The table is the resource System.Globalization.unicode_category.bin (made by
// tools/gen_unicode_category.py from Unicode 16.0): the runs of the code points that have the same category, sorted, so
// that a category is found by a search of the run. The table is read the first time that a category is asked for.
public static class CharUnicodeInfo
{
    // The category of the character.
    public static UnicodeCategory GetUnicodeCategory(Char ch) => (UnicodeCategory)GetCategoryNumber((Int32)ch);

    // The category of the code point (U+0000 to U+10FFFF; a value that is not a code point is not assigned).
    public static UnicodeCategory GetUnicodeCategory(Int32 codePoint) => (UnicodeCategory)GetCategoryNumber(codePoint);

    // The category of the character at the index of the text: a pair of surrogates is the character of the plane that it
    // codes.
    public static UnicodeCategory GetUnicodeCategory(String s, Int32 index)
    {
        if (s == null)
            throw new ArgumentException("s is null");

        if (index < 0 || index >= s.Length)
            throw new OutOfRangeException();

        Char c = s[index];
        if (Char.IsHighSurrogate(c) && index + 1 < s.Length && Char.IsLowSurrogate(s[index + 1]))
            return GetUnicodeCategory(0x10000 + (((Int32)c - 0xD800) << 10) + ((Int32)s[index + 1] - 0xDC00));

        return GetUnicodeCategory(c);
    }

    // The number of the category (see UnicodeCategory) of the code point.
    internal static Int32 GetCategoryNumber(Int32 codePoint)
    {
        if (codePoint < 0 || codePoint > 0x10FFFF)
            return 29;

        // The characters of ASCII are the most of the texts: they have no search.
        if (codePoint < 128)
        {
            if (__ascii == null)
                __LoadAscii();

            return (Int32)__ascii[codePoint];
        }

        UInt32[] runs = __Runs();

        // The last run that starts at the code point or before it.
        Int32 low = 0;
        Int32 high = runs.Length - 1;
        while (low < high)
        {
            Int32 middle = (low + high + 1) / 2;
            if ((Int32)(runs[middle] >> 5) <= codePoint)
                low = middle;
            else
                high = middle - 1;
        }

        return (Int32)(runs[low] & 31);
    }

    // Reads the table: it is made the first time.
    private static UInt32[] __Runs()
    {
        if (__runs != null)
            return __runs;

        UInt8[] data = Runtime.Resource.GetBytes("System.Globalization.unicode_category.bin");

        Int32 count = (Int32)data[0] | ((Int32)data[1] << 8) | ((Int32)data[2] << 16) | ((Int32)data[3] << 24);
        UInt32[] runs = new UInt32[count];

        for (Int32 k = 0; k < count; k++)
        {
            Int32 at = 4 + k * 4;
            runs[k] = (UInt32)((Int32)data[at] | ((Int32)data[at + 1] << 8) | ((Int32)data[at + 2] << 16) | ((Int32)data[at + 3] << 24));
        }

        __runs = runs;
        return runs;
    }

    // The categories of the characters of ASCII, from the table.
    private static Void __LoadAscii()
    {
        UInt32[] runs = __Runs();
        UInt8[] ascii = new UInt8[128];

        Int32 run = 0;
        for (Int32 c = 0; c < 128; c++)
        {
            while (run + 1 < runs.Length && (Int32)(runs[run + 1] >> 5) <= c)
                run++;

            ascii[c] = (UInt8)(runs[run] & 31);
        }

        __ascii = ascii;
    }

    private static UInt32[]? __runs;
    private static UInt8[]? __ascii;
}
