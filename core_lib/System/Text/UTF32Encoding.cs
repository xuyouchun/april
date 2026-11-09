/*
 *  System.dll
 *
 *  UTF32Encoding: UTF-32.
 */

namespace Text;

////////// ////////// ////////// ////////// //////////

// UTF-32 with the least significant byte first: each character is 4 bytes, a pair of surrogates is one
// character. A surrogate that has no pair, a value that is not a character, and the bytes that are left
// at the end are U+FFFD.
public class UTF32Encoding : Encoding
{
    public UTF32Encoding() : base("utf-32")
    {
        __preamble = false;
    }

    // With byteOrderMark, GetPreamble gives U+FEFF (FF FE 00 00).
    public UTF32Encoding(Boolean byteOrderMark) : base("utf-32")
    {
        __preamble = byteOrderMark;
    }

    public override Int32 CodePage => 12000;

    public override String EncodingName => "Unicode (UTF-32)";

    public override UInt8[] GetPreamble() => __preamble ? new UInt8[] { 0xFF, 0xFE, 0, 0 } : new UInt8[0];

    // The bytes that are not a whole character (4 bytes) are not complete.
    public override Int32 GetCompleteLength(UInt8[] bytes, Int32 count) => count - count % 4;

    protected override Int32 CountBytes(String s)
    {
        Int32 length = s.Length, count = 0;

        for (Int32 k = 0; k < length; k++)
        {
            if (__IsPairAt(s, k))
                k++;

            count++;
        }

        return count * 4;
    }

    public override Int32 GetMaxByteCount(Int32 charCount) => (charCount + 1) * 4;

    protected override Encoding MakeCopy() => new UTF32Encoding(__preamble);

    // A character of 4 bytes that is a scalar value is valid; a value that is not is an error; the bytes that
    // are left at the end are one error.
    protected override Int32 CheckBytes(UInt8[] bytes, Int32 index, Int32 end)
    {
        if (end - index < 4)
            return index - end;

        Int32 high = (Int32)bytes[index + 3];
        Int32 c = (Int32)bytes[index] | ((Int32)bytes[index + 1] << 8) | ((Int32)bytes[index + 2] << 16);

        if (high != 0 || c > 0x10FFFF || (c >= 0xD800 && c <= 0xDFFF))
            return -4;

        return 4;
    }

    protected override Int32 WriteBytes(String s, UInt8[] bytes, Int32 index)
    {
        Int32 length = s.Length, start = index;

        for (Int32 k = 0; k < length; k++)
        {
            Int32 c = (Int32)s[k];

            if (__IsPairAt(s, k))
            {
                c = 0x10000 + ((c - 0xD800) << 10) + ((Int32)s[k + 1] - 0xDC00);
                k++;
            }
            else if (c >= 0xD800 && c <= 0xDFFF)
            {
                c = 0xFFFD;
            }

            bytes[index++] = (UInt8)(c & 255);
            bytes[index++] = (UInt8)((c >> 8) & 255);
            bytes[index++] = (UInt8)((c >> 16) & 255);
            bytes[index++] = (UInt8)((c >> 24) & 255);
        }

        return index - start;
    }

    protected override String ReadString(UInt8[] bytes, Int32 index, Int32 count)
    {
        StringBuilder sb = new StringBuilder(count / 4 + 1);

        Int32 end = index + count - count % 4;
        for (Int32 k = index; k < end; k += 4)
        {
            Int32 high = (Int32)bytes[k + 3];
            Int32 c = (Int32)bytes[k] | ((Int32)bytes[k + 1] << 8) | ((Int32)bytes[k + 2] << 16);

            if (high != 0 || c > 0x10FFFF || (c >= 0xD800 && c <= 0xDFFF))
            {
                sb.Append('�');
            }
            else if (c >= 0x10000)
            {
                c -= 0x10000;
                sb.Append((Char)(0xD800 + (c >> 10)));
                sb.Append((Char)(0xDC00 + (c & 0x3FF)));
            }
            else
            {
                sb.Append((Char)c);
            }
        }

        if (count % 4 != 0)
            sb.Append('�');

        return sb.ToString();
    }

    // Whether a pair of surrogates is at the index.
    private Boolean __preamble;

    private static Boolean __IsPairAt(String s, Int32 index)
    {
        if (index + 1 >= s.Length)
            return false;

        Int32 first = (Int32)s[index], second = (Int32)s[index + 1];
        return first >= 0xD800 && first <= 0xDBFF && second >= 0xDC00 && second <= 0xDFFF;
    }
}
