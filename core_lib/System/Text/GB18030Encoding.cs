/*
 *  System.dll
 *
 *  GB18030Encoding: GB 18030, the encoding of the Chinese characters.
 */

namespace Text;

////////// ////////// ////////// ////////// //////////

// GB 18030 (the mapping of the 2005 edition): the characters up to U+007F are a byte; the others are two bytes (the
// lead byte 0x81 to 0xFE, the trail byte 0x40 to 0x7E or 0x80 to 0xFE) or four bytes (0x81 to 0xFE, 0x30 to 0x39,
// 0x81 to 0xFE, 0x30 to 0x39). Every character of Unicode has a code, so that nothing is lost but the surrogates that
// have no pair (U+FFFD). The table is the resource System.Text.gb18030.bin (tools/gen_gb18030.py makes it): it is read
// when an encoding is first used.
//
// The bytes that are not valid are errors of one byte (the lead byte), as the sequence that is complete and is not a
// character (a code of four bytes that is not of a character); the bytes at the end that are the beginning of a code
// are one error.
public class GB18030Encoding : Encoding
{
    public GB18030Encoding() : base("gb18030")
    {
    }

    public override Int32 CodePage => 54936;

    public override String EncodingName => "Chinese Simplified (GB18030)";

    // A character is at most 4 bytes.
    public override Int32 GetMaxByteCount(Int32 charCount) => (charCount + 1) * 4;

    protected override Encoding MakeCopy() => new GB18030Encoding();

    protected override Int32 CountBytes(String s)
    {
        __Load();
        __LoadReverse();

        Int32 length = s.Length, count = 0;

        for (Int32 k = 0; k < length; k++)
        {
            Int32 c = (Int32)s[k];

            if (__IsPairAt(s, k))
            {
                k++;
                count += 4;
            }
            else
            {
                count += __SizeOf(c);
            }
        }

        return count;
    }

    protected override Int32 WriteBytes(String s, UInt8[] bytes, Int32 index)
    {
        __Load();
        __LoadReverse();

        Int32 length = s.Length, start = index;

        for (Int32 k = 0; k < length; k++)
        {
            Int32 c = (Int32)s[k];

            if (__IsPairAt(s, k))
            {
                c = 0x10000 + ((c - 0xD800) << 10) + ((Int32)s[k + 1] - 0xDC00);
                k++;
            }

            index = __Write(c, bytes, index);
        }

        return index - start;
    }

    protected override String ReadString(UInt8[] bytes, Int32 index, Int32 count)
    {
        __Load();

        StringBuilder sb = new StringBuilder(count + 1);
        Int32 end = index + count, k = index;

        while (k < end)
        {
            Int32 n = CheckBytes(bytes, k, end);

            if (n < 0)
            {
                sb.Append((Char)0xFFFD);
                k += -n;
                continue;
            }

            Int32 first = (Int32)bytes[k];

            if (n == 1)
            {
                sb.Append((Char)first);
            }
            else if (n == 2)
            {
                sb.Append((Char)__forward[__IndexOf(first, (Int32)bytes[k + 1])]);
            }
            else
            {
                Int32 c = __CodePointOfLinear(__LinearOf(bytes, k));

                if (c >= 0x10000)
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

            k += n;
        }

        return sb.ToString();
    }

    // The unit that begins at the index: 1, 2 or 4 bytes of a character, or minus the bytes of an error.
    protected override Int32 CheckBytes(UInt8[] bytes, Int32 index, Int32 end)
    {
        __Load();

        Int32 first = (Int32)bytes[index];
        if (first < 0x80)
            return 1;

        if (first == 0x80 || first == 0xFF)
            return -1;

        Int32 left = end - index;
        if (left < 2)
            return -1;

        Int32 second = (Int32)bytes[index + 1];
        if ((second >= 0x40 && second <= 0x7E) || (second >= 0x80 && second <= 0xFE))
            return 2;

        if (second < 0x30 || second > 0x39)
            return -1;

        // Four bytes: the ones that are at the end are one error.
        if (left < 3)
            return -left;

        Int32 third = (Int32)bytes[index + 2];
        if (third < 0x81 || third > 0xFE)
            return -1;

        if (left < 4)
            return -left;

        Int32 fourth = (Int32)bytes[index + 3];
        if (fourth < 0x30 || fourth > 0x39)
            return -1;

        return __CodePointOfLinear(__LinearOf(bytes, index)) >= 0 ? 4 : -4;
    }

    // The bytes at the end that are the beginning of a code wait for the next bytes.
    public override Int32 GetCompleteLength(UInt8[] bytes, Int32 count)
    {
        __Load();

        Int32 k = 0;

        while (k < count)
        {
            if (count - k <= 3 && __IsBeginning(bytes, k, count))
                return k;

            Int32 n = CheckBytes(bytes, k, count);
            k += n > 0 ? n : -n;
        }

        return count;
    }

    // Whether the bytes from the index to the end are the beginning of a code of two or four bytes, that is not complete.
    private static Boolean __IsBeginning(UInt8[] bytes, Int32 index, Int32 end)
    {
        Int32 left = end - index;
        Int32 first = (Int32)bytes[index];

        if (first < 0x81 || first > 0xFE)
            return false;

        if (left == 1)
            return true;

        Int32 second = (Int32)bytes[index + 1];
        if (second < 0x30 || second > 0x39)
            return false;

        if (left == 2)
            return true;

        Int32 third = (Int32)bytes[index + 2];
        return left == 3 && third >= 0x81 && third <= 0xFE;
    }

    // The index in the table of the codes of two bytes.
    private static Int32 __IndexOf(Int32 lead, Int32 trail) => (lead - 0x81) * 190 + (trail < 0x80 ? trail - 0x40 : trail - 0x41);

    // The number of a code of four bytes.
    private static Int32 __LinearOf(UInt8[] bytes, Int32 index)
    {
        return ((((Int32)bytes[index] - 0x81) * 10 + (Int32)bytes[index + 1] - 0x30) * 126 + (Int32)bytes[index + 2] - 0x81) * 10
            + (Int32)bytes[index + 3] - 0x30;
    }

    // The character of the number of a code of four bytes, or -1 when it is not one.
    private static Int32 __CodePointOfLinear(Int32 linear)
    {
        if (linear >= 189000)
        {
            Int32 c = 0x10000 + linear - 189000;
            return c <= 0x10FFFF ? c : -1;
        }

        Int32 range = __RangeBy(0, linear);
        if (range < 0 || linear - __ranges[range * 3] >= __ranges[range * 3 + 2])
            return -1;

        return __ranges[range * 3 + 1] + linear - __ranges[range * 3];
    }

    // The last range of which the value of the field (0: the number of the code, 1: the character) is not over the value,
    // or -1.
    private static Int32 __RangeBy(Int32 field, Int32 value)
    {
        Int32 low = 0, high = __rangeCount - 1, found = -1;

        while (low <= high)
        {
            Int32 middle = (low + high) / 2;

            if (__ranges[middle * 3 + field] <= value)
            {
                found = middle;
                low = middle + 1;
            }
            else
            {
                high = middle - 1;
            }
        }

        return found;
    }

    // The number of bytes of a character of the basic plane (a surrogate that has no pair is U+FFFD).
    private static Int32 __SizeOf(Int32 c)
    {
        if (c < 0x80)
            return 1;

        if (c >= 0xD800 && c <= 0xDFFF)
            c = 0xFFFD;

        return __reverse[c] != 0 ? 2 : 4;
    }

    // Writes the bytes of a character, returns the index after them.
    private static Int32 __Write(Int32 c, UInt8[] bytes, Int32 index)
    {
        if (c < 0x80)
        {
            bytes[index++] = (UInt8)c;
            return index;
        }

        Int32 linear;

        if (c >= 0x10000)
        {
            linear = 189000 + c - 0x10000;
        }
        else
        {
            if (c >= 0xD800 && c <= 0xDFFF)
                c = 0xFFFD;

            Int32 code = (Int32)__reverse[c];

            if (code != 0)
            {
                bytes[index++] = (UInt8)(code >> 8);
                bytes[index++] = (UInt8)(code & 255);
                return index;
            }

            Int32 range = __RangeBy(1, c);
            linear = __ranges[range * 3] + c - __ranges[range * 3 + 1];
        }

        bytes[index + 3] = (UInt8)(0x30 + linear % 10);
        linear /= 10;
        bytes[index + 2] = (UInt8)(0x81 + linear % 126);
        linear /= 126;
        bytes[index + 1] = (UInt8)(0x30 + linear % 10);
        bytes[index] = (UInt8)(0x81 + linear / 10);

        return index + 4;
    }

    // Whether a pair of surrogates is at the index.
    private static Boolean __IsPairAt(String s, Int32 index)
    {
        if (index + 1 >= s.Length)
            return false;

        Int32 first = (Int32)s[index], second = (Int32)s[index + 1];
        return first >= 0xD800 && first <= 0xDBFF && second >= 0xDC00 && second <= 0xDFFF;
    }

    // Reads the table: it is made the first time.
    private static Void __Load()
    {
        if (__forward != null)
            return;

        UInt8[] data = Runtime.Resource.GetBytes("System.Text.gb18030.bin");

        Int32 count = __Int32At(data, 0);
        UInt16[] forward = new UInt16[count];

        for (Int32 k = 0; k < count; k++)
            forward[k] = (UInt16)((Int32)data[4 + k * 2] | ((Int32)data[5 + k * 2] << 8));

        Int32 at = 4 + count * 2;
        Int32 rangeCount = __Int32At(data, at);
        Int32[] ranges = new Int32[rangeCount * 3];

        for (Int32 k = 0; k < rangeCount * 3; k++)
            ranges[k] = __Int32At(data, at + 4 + k * 4);

        __forwardCount = count;
        __rangeCount = rangeCount;
        __ranges = ranges;
        __forward = forward;
    }

    // The table from the character to the code of two bytes: it is made when the first text is coded.
    private static Void __LoadReverse()
    {
        if (__reverse != null)
            return;

        __Load();

        UInt16[] reverse = new UInt16[65536];

        for (Int32 k = 0; k < __forwardCount; k++)
        {
            Int32 lead = 0x81 + k / 190, trail = k % 190;
            trail = trail < 63 ? 0x40 + trail : 0x41 + trail;

            reverse[(Int32)__forward[k]] = (UInt16)((lead << 8) | trail);
        }

        __reverse = reverse;
    }

    private static Int32 __Int32At(UInt8[] data, Int32 index)
    {
        return (Int32)data[index] | ((Int32)data[index + 1] << 8) | ((Int32)data[index + 2] << 16) | ((Int32)data[index + 3] << 24);
    }

    // The characters of the codes of two bytes, and the ranges of the codes of four bytes (linear start, character start,
    // count).
    private static UInt16[]? __forward;

    private static UInt16[]? __reverse;

    private static Int32[]? __ranges;

    private static Int32 __forwardCount;

    private static Int32 __rangeCount;
}
