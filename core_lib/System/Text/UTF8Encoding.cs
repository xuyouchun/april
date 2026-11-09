/*
 *  System.dll
 *
 *  UTF8Encoding: UTF-8.
 */

namespace Text;

////////// ////////// ////////// ////////// //////////

// UTF-8, made by internal functions. The bytes that are not UTF-8 (a sequence that is too long, a
// surrogate, a code point over U+10FFFF, a sequence that is not complete) are U+FFFD; so is a surrogate
// that has no pair, when the text is coded.
public class UTF8Encoding : Encoding
{
    public UTF8Encoding() : base("utf-8")
    {
        __preamble = false;
    }

    // With emitPreamble, GetPreamble gives the byte order mark (EF BB BF).
    public UTF8Encoding(Boolean emitPreamble) : base("utf-8")
    {
        __preamble = emitPreamble;
    }

    public override Int32 CodePage => 65001;

    public override String EncodingName => "Unicode (UTF-8)";

    public override UInt8[] GetPreamble() => __preamble ? new UInt8[] { 0xEF, 0xBB, 0xBF } : new UInt8[0];

    // The bytes of a sequence that is cut at the end (a lead byte that can start a sequence, and the
    // continuation bytes that follow it) are not complete.
    public override Int32 GetCompleteLength(UInt8[] bytes, Int32 count)
    {
        Int32 k = count - 1, back = 0;

        while (k >= 0 && back < 3 && ((Int32)bytes[k] & 0xC0) == 0x80)
        {
            k--;
            back++;
        }

        if (k < 0)
            return count;

        Int32 lead = (Int32)bytes[k];
        Int32 need = lead >= 0xF0 && lead <= 0xF4 ? 4 : (lead >= 0xE0 && lead <= 0xEF ? 3 : (lead >= 0xC2 && lead <= 0xDF ? 2 : 1));

        return count - k < need ? k : count;
    }

    protected override Int32 CountBytes(String s) => Internal.Utf8_GetByteCount(s);

    protected override Encoding MakeCopy() => new UTF8Encoding(__preamble);

    // The same units as the ones of Utf8_GetString: a lead byte that is not one (80 to C1, F5 to FF) is an error;
    // a sequence that is not complete is one error with its good bytes; a sequence that is too long, a surrogate or
    // a value over U+10FFFF is an error of the lead byte, the bytes that follow are errors of their own.
    protected override Int32 CheckBytes(UInt8[] bytes, Int32 index, Int32 end)
    {
        Int32 b = (Int32)bytes[index];
        Int32 need, c, min;

        if (b < 0x80)
            return 1;

        if (b >= 0xC2 && b <= 0xDF)
        {
            need = 1;
            c = b & 0x1F;
            min = 0x80;
        }
        else if (b >= 0xE0 && b <= 0xEF)
        {
            need = 2;
            c = b & 0x0F;
            min = 0x800;
        }
        else if (b >= 0xF0 && b <= 0xF4)
        {
            need = 3;
            c = b & 0x07;
            min = 0x10000;
        }
        else
        {
            return -1;
        }

        Int32 used = 1;
        while (used <= need && index + used < end && ((Int32)bytes[index + used] & 0xC0) == 0x80)
        {
            c = (c << 6) | ((Int32)bytes[index + used] & 0x3F);
            used++;
        }

        if (used <= need)
            return -used;

        if (c < min || c > 0x10FFFF || (c >= 0xD800 && c <= 0xDFFF))
            return -1;

        return used;
    }

    // A character is at most 3 bytes (a pair is 4 bytes for 2 units).
    public override Int32 GetMaxByteCount(Int32 charCount) => (charCount + 1) * 3;

    protected override Int32 WriteBytes(String s, UInt8[] bytes, Int32 index) => Internal.Utf8_GetBytes(s, bytes, index);

    protected override String ReadString(UInt8[] bytes, Int32 index, Int32 count) => Internal.Utf8_GetString(bytes, index, count);

    private Boolean __preamble;
}
