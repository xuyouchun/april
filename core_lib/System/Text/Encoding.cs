/*
 *  System.dll
 *
 *  Encoding: the conversion between the text (strings, characters) and the bytes.
 */

namespace Text;

////////// ////////// ////////// ////////// //////////

// The base of the encodings. A character that an encoding cannot code is replaced (U+FFFD in the Unicode
// encodings, '?' in ASCII and Latin1), so that the conversions never fail because of the text; the ranges
// that are not in an array are errors (OutOfRangeException).
public abstract class Encoding
{
    protected Encoding(String name)
    {
        __name = name;
    }

    // The name of the encoding: utf-8, utf-16, utf-16BE, utf-32, us-ascii, iso-8859-1.
    public String Name => __name;

    // The name that is used on the web (the same as Name).
    public String WebName => __name;

    // The number of the code page: 65001 (UTF-8), 1200 (UTF-16), 1201 (UTF-16BE), 12000 (UTF-32), 20127 (ASCII),
    // 28591 (Latin1).
    public virtual Int32 CodePage => 0;

    // The name that people read: Unicode (UTF-8).
    public virtual String EncodingName => __name;

    // Whether a character is one byte (ASCII, Latin1).
    public virtual Boolean IsSingleByte => false;

    // What is done with a character that cannot be coded: the replacement of the encoding (U+FFFD or '?') until
    // another fallback is given.
    public EncoderFallback EncoderFallback
    {
        get
        {
            if (__fallback != null)
                return __fallback;

            return new EncoderReplacementFallback(DefaultReplacement);
        }
    }

    // Makes an encoding like this one that handles the characters that cannot be coded by the fallback, for
    // instance EncoderFallback.ExceptionFallback. This encoding is not changed.
    public Encoding WithEncoderFallback(EncoderFallback fallback)
    {
        Encoding copy = MakeCopy();
        copy.__fallback = fallback;
        copy.__decoderFallback = __decoderFallback;
        return copy;
    }

    // What is done with bytes that cannot be decoded: the replacement of the encoding (U+FFFD or '?') until
    // another fallback is given.
    public DecoderFallback DecoderFallback
    {
        get
        {
            if (__decoderFallback != null)
                return __decoderFallback;

            return new DecoderReplacementFallback(DefaultReplacement);
        }
    }

    // Makes an encoding like this one that handles the bytes that cannot be decoded by the fallback, for
    // instance DecoderFallback.ExceptionFallback. This encoding is not changed (its encoder fallback is kept).
    public Encoding WithDecoderFallback(DecoderFallback fallback)
    {
        Encoding copy = MakeCopy();
        copy.__fallback = __fallback;
        copy.__decoderFallback = fallback;
        return copy;
    }

    // The number of bytes that the string has.
    public Int32 GetByteCount(String s) => CountBytes(__Replace(s));

    // The largest number of bytes that a number of characters can have.
    public abstract Int32 GetMaxByteCount(Int32 charCount);

    // Writes the bytes of the string into the array at the index, returns the number of bytes.
    public Int32 GetBytes(String s, UInt8[] bytes, Int32 index)
    {
        String text = __Replace(s);
        if (index < 0 || index > bytes.Length - CountBytes(text))
            throw new OutOfRangeException();

        return WriteBytes(text, bytes, index);
    }

    // The bytes of the string.
    public UInt8[] GetBytes(String s)
    {
        String text = __Replace(s);
        UInt8[] bytes = new UInt8[CountBytes(text)];
        WriteBytes(text, bytes, 0);
        return bytes;
    }

    public UInt8[] GetBytes(Char[] chars) => GetBytes(String.FromChars(chars));

    public Int32 GetByteCount(Char[] chars) => GetByteCount(String.FromChars(chars));

    // The string of the bytes of the count from the index.
    public String GetString(UInt8[] bytes, Int32 index, Int32 count)
    {
        if (index < 0 || count < 0 || index > bytes.Length - count)
            throw new OutOfRangeException();

        return __Decode(bytes, index, count);
    }

    public String GetString(UInt8[] bytes) => __Decode(bytes, 0, bytes.Length);

    // The characters of the bytes.
    public Char[] GetChars(UInt8[] bytes) => GetString(bytes).ToCharArray();

    // The number of characters (UTF-16 units) that the bytes have.
    public Int32 GetCharCount(UInt8[] bytes) => GetString(bytes).Length;

    // The bytes that a file starts with to tell the encoding (the byte order mark); none by default. The
    // GetBytes functions do not write it.
    public virtual UInt8[] GetPreamble() => new UInt8[0];

    // The number of bytes at the start of the bytes that make whole characters: the others (the beginning of a
    // character that is cut at the end) wait for the next bytes. A decoder uses it to convert in parts.
    public virtual Int32 GetCompleteLength(UInt8[] bytes, Int32 count) => count;

    // The objects that convert in parts.
    public Encoder GetEncoder() => new Encoder(this);

    public Decoder GetDecoder() => new Decoder(this);

    // The number of bytes of the string.
    protected abstract Int32 CountBytes(String s);

    // Whether the encoding codes the character: a code point, or a surrogate that has no pair (one unit).
    // The ones that cannot be coded are handled by the fallback. Surrogates that have no pair cannot be coded
    // by default.
    protected virtual Boolean CanEncode(Int32 codePoint) => codePoint < 0xD800 || codePoint > 0xDFFF;

    // The string that replaces what cannot be coded when no fallback is given.
    protected virtual String DefaultReplacement => "�";

    // An encoding like this one, that has no fallback of its own.
    protected virtual Encoding MakeCopy()
    {
        throw new NotSupportedException();
    }

    // The number of bytes of the unit that begins at the index of the bytes that end at end: a positive number is a
    // unit that is valid (a character), a negative number is minus the number of the bytes of one error (the bytes
    // that cannot be decoded). The fallback handles the errors. All the bytes are valid by default.
    protected virtual Int32 CheckBytes(UInt8[] bytes, Int32 index, Int32 end) => 1;

    // The string of the bytes: the errors are handled by the fallback, when there is one.
    private String __Decode(UInt8[] bytes, Int32 index, Int32 count)
    {
        if (__decoderFallback == null)
            return ReadString(bytes, index, count);

        StringBuilder? result = null;
        Int32 end = index + count, start = index, k = index;

        while (k < end)
        {
            Int32 n = CheckBytes(bytes, k, end);
            if (n > 0)
            {
                k += n;
                continue;
            }

            n = -n;
            if (result == null)
                result = new StringBuilder();

            if (k > start)
                result.Append(ReadString(bytes, start, k - start));

            UInt8[] unknown = new UInt8[n];
            for (Int32 j = 0; j < n; j++)
                unknown[j] = bytes[k + j];

            result.Append(__decoderFallback.GetReplacement(unknown, k));

            k += n;
            start = k;
        }

        if (result == null)
            return ReadString(bytes, index, count);

        if (end > start)
            result.Append(ReadString(bytes, start, end - start));

        return result.ToString();
    }

    // The string with the characters that cannot be coded replaced by those of the fallback, or the string
    // itself when there is no fallback.
    private String __Replace(String s)
    {
        if (__fallback == null)
            return s;

        StringBuilder? result = null;
        Int32 copied = 0;

        for (Int32 k = 0; k < s.Length; k++)
        {
            Int32 length = 1;
            Int32 c = (Int32)s[k];

            if (Char.IsHighSurrogate(s[k]) && k + 1 < s.Length && Char.IsLowSurrogate(s[k + 1]))
            {
                length = 2;
                c = 0x10000 + ((c - 0xD800) << 10) + ((Int32)s[k + 1] - 0xDC00);
            }

            if (CanEncode(c))
            {
                k += length - 1;
                continue;
            }

            if (result == null)
                result = new StringBuilder();

            result.Append(s, copied, k - copied);
            result.Append(__fallback.GetReplacement(s.Substring(k, length), k));

            k += length - 1;
            copied = k + 1;
        }

        if (result == null)
            return s;

        result.Append(s, copied, s.Length - copied);
        return result.ToString();
    }

    // Writes the bytes of the string at the index (the room is checked), returns the number of bytes.
    protected abstract Int32 WriteBytes(String s, UInt8[] bytes, Int32 index);

    // Makes the string of the bytes (the range is checked).
    protected abstract String ReadString(UInt8[] bytes, Int32 index, Int32 count);

    // The encoding of the name (the case does not matter, some other names are known: utf8, ascii, latin1, unicode),
    // the same object as the one of the property (Encoding.UTF8). An ArgumentException when it is not known.
    public static Encoding GetEncoding(String name)
    {
        Encoding? encoding = __Find(name);
        if (encoding == null)
            throw new ArgumentException("the encoding '" + name + "' is not known");

        return encoding;
    }

    // The encoding of the code page; an ArgumentException when it is not known.
    public static Encoding GetEncoding(Int32 codePage)
    {
        Encoding? encoding = __Find(codePage);
        if (encoding == null)
            throw new ArgumentException("the code page " + codePage + " is not known");

        return encoding;
    }

    // The encoding with the fallbacks (the encoding of the property is not changed).
    public static Encoding GetEncoding(String name, EncoderFallback encoderFallback, DecoderFallback decoderFallback)
        => GetEncoding(name).WithEncoderFallback(encoderFallback).WithDecoderFallback(decoderFallback);

    public static Encoding GetEncoding(Int32 codePage, EncoderFallback encoderFallback, DecoderFallback decoderFallback)
        => GetEncoding(codePage).WithEncoderFallback(encoderFallback).WithDecoderFallback(decoderFallback);

    // The encoding that is used when none is given: UTF-8.
    public static Encoding Default => UTF8;

    // The bytes of the text of the bytes of an encoding, in another encoding.
    public static UInt8[] Convert(Encoding source, Encoding destination, UInt8[] bytes)
        => destination.GetBytes(source.GetString(bytes));

    public static UInt8[] Convert(Encoding source, Encoding destination, UInt8[] bytes, Int32 index, Int32 count)
        => destination.GetBytes(source.GetString(bytes, index, count));

    private static Encoding? __Find(String name)
    {
        String n = name.Trim().ToLower();

        if (n == "utf-8" || n == "utf8" || n == "unicode-1-1-utf-8" || n == "utf_8")
            return UTF8;

        if (n == "utf-16" || n == "utf16" || n == "utf-16le" || n == "unicode" || n == "ucs-2" || n == "ucs-2le")
            return Unicode;

        if (n == "utf-16be" || n == "unicodefffe" || n == "ucs-2be")
            return BigEndianUnicode;

        if (n == "utf-32" || n == "utf32" || n == "utf-32le")
            return UTF32;

        if (n == "us-ascii" || n == "ascii" || n == "ansi_x3.4-1968" || n == "iso646-us" || n == "us")
            return ASCII;

        if (n == "gb18030" || n == "gb-18030" || n == "x-gb18030" || n == "cp54936")
            return GB18030;

        if (n == "iso-8859-1" || n == "latin1" || n == "latin-1" || n == "iso8859-1" || n == "l1" || n == "cp819" || n == "ibm819")
            return Latin1;

        return null;
    }

    private static Encoding? __Find(Int32 codePage)
    {
        if (codePage == 65001)
            return UTF8;

        if (codePage == 1200)
            return Unicode;

        if (codePage == 1201)
            return BigEndianUnicode;

        if (codePage == 12000)
            return UTF32;

        if (codePage == 20127)
            return ASCII;

        if (codePage == 28591)
            return Latin1;

        if (codePage == 54936)
            return GB18030;

        return null;
    }

    // The encodings. They are made when they are first asked for: the static fields of a library have no
    // initial values (the static constructors are called by the entry point of the program).
    public static Encoding UTF8
    {
        get
        {
            if (__utf8 == null)
                __utf8 = new UTF8Encoding();

            return __utf8;
        }
    }

    // UTF-16, the least significant byte first.
    public static Encoding Unicode
    {
        get
        {
            if (__unicode == null)
                __unicode = new UnicodeEncoding(false);

            return __unicode;
        }
    }

    // UTF-16, the most significant byte first.
    public static Encoding BigEndianUnicode
    {
        get
        {
            if (__bigEndianUnicode == null)
                __bigEndianUnicode = new UnicodeEncoding(true);

            return __bigEndianUnicode;
        }
    }

    // UTF-32, the least significant byte first.
    public static Encoding UTF32
    {
        get
        {
            if (__utf32 == null)
                __utf32 = new UTF32Encoding();

            return __utf32;
        }
    }

    // The characters up to U+007F, the others are '?'.
    public static Encoding ASCII
    {
        get
        {
            if (__ascii == null)
                __ascii = new ASCIIEncoding();

            return __ascii;
        }
    }

    // GB 18030, the Chinese characters (the table is the resource System.Text.gb18030.bin).
    public static Encoding GB18030
    {
        get
        {
            if (__gb18030 == null)
                __gb18030 = new GB18030Encoding();

            return __gb18030;
        }
    }

    // ISO 8859-1: the characters up to U+00FF, the others are '?'.
    public static Encoding Latin1
    {
        get
        {
            if (__latin1 == null)
                __latin1 = new Latin1Encoding();

            return __latin1;
        }
    }

    private String __name;

    private EncoderFallback? __fallback;

    private DecoderFallback? __decoderFallback;

    private static Encoding? __utf8;

    private static Encoding? __unicode;

    private static Encoding? __bigEndianUnicode;

    private static Encoding? __utf32;

    private static Encoding? __ascii;

    private static Encoding? __latin1;

    private static Encoding? __gb18030;
}
