/*
 *  System.dll
 *
 *  Encoder: the conversion of the text to bytes, in parts.
 */

namespace Text;

////////// ////////// ////////// ////////// //////////

// Converts the text that comes in parts (the blocks of a file or a stream): a high surrogate at the end of a
// part is kept until the next part, so that a pair that is cut is coded as one character. The bytes of all the
// parts are the bytes of the whole text. The last part is given with flush = true: a high surrogate that is
// left is a character that cannot be coded.
public class Encoder
{
    public Encoder(Encoding encoding)
    {
        __encoding = encoding;
        __hasPending = false;
        __pending = '\0';
    }

    public Encoding Encoding => __encoding;

    // The number of bytes of the text and what is kept from the previous parts. The state is not changed.
    public Int32 GetByteCount(String s, Boolean flush)
    {
        return __encoding.GetByteCount(__Take(s, flush, false));
    }

    // The bytes of what is kept from the previous parts and the text. A high surrogate at the end is kept
    // unless it is the last part.
    public UInt8[] GetBytes(String s, Boolean flush)
    {
        return __encoding.GetBytes(__Take(s, flush, true));
    }

    public UInt8[] GetBytes(Char[] chars, Int32 index, Int32 count, Boolean flush)
    {
        if (index < 0 || count < 0 || index > chars.Length - count)
            throw new OutOfRangeException();

        return GetBytes(String.FromChars(chars).Substring(index, count), flush);
    }

    // Forgets what is kept.
    public Void Reset()
    {
        __hasPending = false;
        __pending = '\0';
    }

    private String __Take(String s, Boolean flush, Boolean change)
    {
        String text = __hasPending ? __pending.ToString() + s : s;
        Boolean hold = false;
        Char last = '\0';

        if (!flush && text.Length > 0 && Char.IsHighSurrogate(text[text.Length - 1]))
        {
            hold = true;
            last = text[text.Length - 1];
            text = text.Substring(0, text.Length - 1);
        }

        if (change)
        {
            __hasPending = hold;
            __pending = last;
        }

        return text;
    }

    private Encoding __encoding;

    private Boolean __hasPending;

    private Char __pending;
}
