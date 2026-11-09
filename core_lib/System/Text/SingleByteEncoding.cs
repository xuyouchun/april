/*
 *  System.dll
 *
 *  SingleByteEncoding: the encodings of which a character is a byte (ASCII, Latin1).
 */

namespace Text;

////////// ////////// ////////// ////////// //////////

// The characters up to a limit are the bytes of the same value, the others are '?' (a pair of
// surrogates is one '?'; the bytes over the limit are '?' too).
public class SingleByteEncoding : Encoding
{
    protected SingleByteEncoding(String name, Int32 limit) : base(name)
    {
        __limit = limit;
    }

    protected override Int32 CountBytes(String s)
    {
        Int32 length = s.Length, count = 0;

        for (Int32 k = 0; k < length; k++)
        {
            if (__IsPairAt(s, k))
                k++;

            count++;
        }

        return count;
    }

    public override Int32 GetMaxByteCount(Int32 charCount) => charCount + 1;

    // The characters up to the limit; a pair of surrogates is not coded.
    protected override Boolean CanEncode(Int32 codePoint) => codePoint <= __limit;

    protected override String DefaultReplacement => "?";

    public override Boolean IsSingleByte => true;

    // A byte over the limit is an error.
    protected override Int32 CheckBytes(UInt8[] bytes, Int32 index, Int32 end) => (Int32)bytes[index] <= __limit ? 1 : -1;

    protected override Int32 WriteBytes(String s, UInt8[] bytes, Int32 index)
    {
        Int32 length = s.Length, start = index;

        for (Int32 k = 0; k < length; k++)
        {
            Int32 c = (Int32)s[k];

            if (__IsPairAt(s, k))
            {
                c = 63;
                k++;
            }

            bytes[index++] = (UInt8)(c <= __limit? c : 63);
        }

        return index - start;
    }

    protected override String ReadString(UInt8[] bytes, Int32 index, Int32 count)
    {
        Char[] chars = new Char[count];

        for (Int32 k = 0; k < count; k++)
        {
            Int32 b = (Int32)bytes[index + k];
            chars[k] = (Char)(b <= __limit? b : 63);
        }

        return String.FromChars(chars);
    }

    private Int32 __limit;

    // Whether a pair of surrogates is at the index.
    private static Boolean __IsPairAt(String s, Int32 index)
    {
        if (index + 1 >= s.Length)
            return false;

        Int32 first = (Int32)s[index], second = (Int32)s[index + 1];
        return first >= 0xD800 && first <= 0xDBFF && second >= 0xDC00 && second <= 0xDFFF;
    }
}

////////// ////////// ////////// ////////// //////////

// US-ASCII: the characters up to U+007F.
public class ASCIIEncoding : SingleByteEncoding
{
    public ASCIIEncoding() : base("us-ascii", 127)
    {
    }

    protected override Encoding MakeCopy() => new ASCIIEncoding();

    public override Int32 CodePage => 20127;

    public override String EncodingName => "US-ASCII";
}

////////// ////////// ////////// ////////// //////////

// ISO 8859-1: the characters up to U+00FF.
public class Latin1Encoding : SingleByteEncoding
{
    public Latin1Encoding() : base("iso-8859-1", 255)
    {
    }

    protected override Encoding MakeCopy() => new Latin1Encoding();

    public override Int32 CodePage => 28591;

    public override String EncodingName => "Western European (ISO)";
}
