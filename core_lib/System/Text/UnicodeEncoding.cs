/*
 *  System.dll
 *
 *  UnicodeEncoding: UTF-16.
 */

namespace Text;

////////// ////////// ////////// ////////// //////////

// UTF-16: each character (unit) is 2 bytes, with the least or the most significant byte first. The units
// are kept as they are (a surrogate that has no pair too); a byte that is left at the end is U+FFFD.
public class UnicodeEncoding : Encoding
{
    public UnicodeEncoding(Boolean bigEndian) : base(bigEndian? "utf-16BE" : "utf-16")
    {
        __bigEndian = bigEndian;
        __preamble = false;
    }

    // With byteOrderMark, GetPreamble gives U+FEFF in the order of the encoding (FF FE or FE FF).
    public UnicodeEncoding(Boolean bigEndian, Boolean byteOrderMark) : base(bigEndian? "utf-16BE" : "utf-16")
    {
        __bigEndian = bigEndian;
        __preamble = byteOrderMark;
    }

    public override Int32 CodePage => __bigEndian ? 1201 : 1200;

    public override String EncodingName => __bigEndian ? "Unicode (Big-Endian)" : "Unicode";

    public override UInt8[] GetPreamble()
    {
        if (!__preamble)
            return new UInt8[0];

        return __bigEndian ? new UInt8[] { 0xFE, 0xFF } : new UInt8[] { 0xFF, 0xFE };
    }

    // A byte that has no second half is not complete.
    public override Int32 GetCompleteLength(UInt8[] bytes, Int32 count) => count - count % 2;

    protected override Int32 CountBytes(String s) => s.Length * 2;

    // The units are kept as they are, a surrogate that has no pair too.
    protected override Boolean CanEncode(Int32 codePoint) => true;

    protected override Encoding MakeCopy() => new UnicodeEncoding(__bigEndian, __preamble);

    // Every unit is valid, a surrogate that has no pair too; a byte that is left at the end is an error.
    protected override Int32 CheckBytes(UInt8[] bytes, Int32 index, Int32 end) => index + 1 < end ? 2 : -1;

    public override Int32 GetMaxByteCount(Int32 charCount) => (charCount + 1) * 2;

    protected override Int32 WriteBytes(String s, UInt8[] bytes, Int32 index)
    {
        Int32 length = s.Length;

        for (Int32 k = 0; k < length; k++)
        {
            Int32 c = (Int32)s[k];
            UInt8 high = (UInt8)(c >> 8);
            UInt8 low = (UInt8)(c & 255);

            if (__bigEndian)
            {
                bytes[index++] = high;
                bytes[index++] = low;
            }
            else
            {
                bytes[index++] = low;
                bytes[index++] = high;
            }
        }

        return length * 2;
    }

    protected override String ReadString(UInt8[] bytes, Int32 index, Int32 count)
    {
        Int32 length = count / 2;
        Char[] chars = new Char[length + count % 2];

        for (Int32 k = 0; k < length; k++)
        {
            Int32 first = (Int32)bytes[index + k * 2];
            Int32 second = (Int32)bytes[index + k * 2 + 1];

            chars[k] = (Char)(__bigEndian? (first << 8) | second : (second << 8) | first);
        }

        if (count % 2 != 0)
            chars[length] = '�';

        return String.FromChars(chars);
    }

    private Boolean __bigEndian;

    private Boolean __preamble;
}
