/*
 *  System.dll
 *
 *  RegexCharClass: a set of characters of a regular expression: [a-z], \d, [^\s], [a-z-[aeiou]].
 */

namespace Text.RegularExpressions;

using Collections;

////////// ////////// ////////// ////////// //////////

// A set of characters: ranges, the classes \d \w \s (and \D \W \S), taken away (Negated) and with a part
// taken out (Subtract). The ranges are sorted and joined by Seal, which is called before the set is used.
internal class RegexCharClass
{
    internal const Int32 Digit = 1;
    internal const Int32 NotDigit = 2;
    internal const Int32 Word = 3;
    internal const Int32 NotWord = 4;
    internal const Int32 Space = 5;
    internal const Int32 NotSpace = 6;

    internal RegexCharClass()
    {
        __lo = new List<Int32>();
        __hi = new List<Int32>();
        __cats = new List<Int32>();
    }

    // A set of the characters that are not in it.
    internal Boolean Negated;

    // Characters that are matched with the other case too.
    internal Boolean IgnoreCase;

    // The characters of this set that are in the other are not in the result.
    internal RegexCharClass? Subtract;

    internal Void AddChar(Int32 c)
    {
        __lo.Add(c);
        __hi.Add(c);
    }

    internal Void AddRange(Int32 lo, Int32 hi)
    {
        __lo.Add(lo);
        __hi.Add(hi);
    }

    // One of the classes: Digit, NotDigit, Word, NotWord, Space, NotSpace.
    internal Void AddCategory(Int32 category)
    {
        __cats.Add(category);
    }

    // Sorts the ranges and joins those that touch each other.
    internal Void Seal()
    {
        Int32 n = __lo.Count;

        // Insertion sort by the low end.
        for (Int32 i = 1; i < n; i++)
        {
            Int32 lo = __lo[i];
            Int32 hi = __hi[i];
            Int32 j = i - 1;
            while (j >= 0 && __lo[j] > lo)
            {
                __lo[j + 1] = __lo[j];
                __hi[j + 1] = __hi[j];
                j--;
            }

            __lo[j + 1] = lo;
            __hi[j + 1] = hi;
        }

        Int32[] rlo = new Int32[n];
        Int32[] rhi = new Int32[n];
        Int32 count = 0;
        for (Int32 i = 0; i < n; i++)
        {
            if (count > 0 && __lo[i] <= rhi[count - 1] + 1)
            {
                if (__hi[i] > rhi[count - 1])
                    rhi[count - 1] = __hi[i];
            }
            else
            {
                rlo[count] = __lo[i];
                rhi[count] = __hi[i];
                count++;
            }
        }

        __rlo = rlo;
        __rhi = rhi;
        __count = count;
        __catArray = __cats.ToArray();

        if (Subtract != null)
            Subtract.Seal();
    }

    // Whether the character is in the set.
    internal Boolean Contains(Char c)
    {
        Boolean r = In(c);
        if (!r && IgnoreCase)
        {
            Char lower = Char.ToLower(c);
            Char upper = Char.ToUpper(c);
            r = (lower != c && In(lower)) || (upper != c && In(upper));
        }

        r = r != Negated;
        if (r && Subtract != null && Subtract.Contains(c))
            return false;

        return r;
    }

    // Whether the character is a letter, a digit or _ (the characters of \w).
    internal static Boolean IsWordChar(Char c)
    {
        return Char.IsLetterOrDigit(c) || c == '_';
    }

    private Boolean In(Char c)
    {
        Int32 v = (Int32)c;

        // Binary search of the range.
        Int32 a = 0;
        Int32 b = __count - 1;
        while (a <= b)
        {
            Int32 m = (a + b) / 2;
            if (v < __rlo[m])
                b = m - 1;
            else if (v > __rhi[m])
                a = m + 1;
            else
                return true;
        }

        for (Int32 i = 0; i < __catArray.Length; i++)
        {
            Int32 cat = __catArray[i];
            Boolean r;
            if (cat == Digit || cat == NotDigit)
                r = Char.IsDigit(c) == (cat == Digit);
            else if (cat == Word || cat == NotWord)
                r = IsWordChar(c) == (cat == Word);
            else
                r = Char.IsWhiteSpace(c) == (cat == Space);

            if (r)
                return true;
        }

        return false;
    }

    private List<Int32> __lo;
    private List<Int32> __hi;
    private List<Int32> __cats;
    private Int32[] __rlo;
    private Int32[] __rhi;
    private Int32 __count;
    private Int32[] __catArray;
}
