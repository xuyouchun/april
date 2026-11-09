/*
 *  System.dll
 *
 *  Decoder: the conversion of bytes to text, in parts.
 */

namespace Text;

////////// ////////// ////////// ////////// //////////

// Converts the bytes that come in parts (the blocks of a file or a stream): the bytes at the end of a part that
// are the beginning of a character (the first bytes of a UTF-8 sequence, a half of a UTF-16 unit) are kept
// until the next part. The text of all the parts is the text of all the bytes. The last part is given with
// flush = true: the bytes that are left are a character that cannot be decoded (U+FFFD).
public class Decoder
{
    public Decoder(Encoding encoding)
    {
        __encoding = encoding;
        __pending = new UInt8[0];
    }

    public Encoding Encoding => __encoding;

    // The number of bytes that are kept for the next part.
    public Int32 PendingByteCount => __pending.Length;

    public String GetString(UInt8[] bytes, Boolean flush) => GetString(bytes, 0, bytes.Length, flush);

    // The text of what is kept from the previous parts and the bytes of the count from the index.
    public String GetString(UInt8[] bytes, Int32 index, Int32 count, Boolean flush)
    {
        if (index < 0 || count < 0 || index > bytes.Length - count)
            throw new OutOfRangeException();

        Int32 kept = __pending.Length, total = kept + count;
        UInt8[] all = new UInt8[total];

        for (Int32 k = 0; k < kept; k++)
            all[k] = __pending[k];

        for (Int32 k = 0; k < count; k++)
            all[kept + k] = bytes[index + k];

        Int32 use = flush ? total : __encoding.GetCompleteLength(all, total);
        UInt8[] rest = new UInt8[total - use];

        for (Int32 k = 0; k < rest.Length; k++)
            rest[k] = all[use + k];

        __pending = rest;
        return __encoding.GetString(all, 0, use);
    }

    public Char[] GetChars(UInt8[] bytes, Int32 index, Int32 count, Boolean flush)
        => GetString(bytes, index, count, flush).ToCharArray();

    // Forgets what is kept.
    public Void Reset()
    {
        __pending = new UInt8[0];
    }

    private Encoding __encoding;

    private UInt8[] __pending;
}
