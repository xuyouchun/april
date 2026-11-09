/*
 *  System.dll
 *
 *  DecoderFallback: what an encoding does with bytes that it cannot decode.
 */

namespace Text;

////////// ////////// ////////// ////////// //////////

// The way that an encoding handles the bytes that are not valid: a UTF-8 sequence that is too long, is not complete,
// is a surrogate or is over U+10FFFF, a character of UTF-32 that is not a scalar value, the byte that is left at the
// end of UTF-16 or UTF-32, a byte over the limit of ASCII. An encoding has the replacement of its own (U+FFFD or '?')
// until a fallback is given by Encoding.WithDecoderFallback. A new fallback derives from this class and gives
// GetReplacement.
public abstract class DecoderFallback
{
    // The text that is decoded in place of the unknown bytes (the bytes of one error: the number of them is the one
    // that the encoding takes for an error), that begin at the index of the array that is decoded. An exception is
    // raised when the bytes cannot be replaced.
    public abstract String GetReplacement(UInt8[] bytesUnknown, Int32 index);

    // The replacement is U+FFFD.
    public static DecoderFallback ReplacementFallback
    {
        get
        {
            if (__replacement == null)
                __replacement = new DecoderReplacementFallback();

            return __replacement;
        }
    }

    // A DecoderFallbackException is raised.
    public static DecoderFallback ExceptionFallback
    {
        get
        {
            if (__exception == null)
                __exception = new DecoderExceptionFallback();

            return __exception;
        }
    }

    private static DecoderFallback? __replacement;

    private static DecoderFallback? __exception;
}

////////// ////////// ////////// ////////// //////////

// The unknown bytes are replaced by a string (U+FFFD unless another one is given).
public class DecoderReplacementFallback : DecoderFallback
{
    public DecoderReplacementFallback() : this("�")
    {
    }

    public DecoderReplacementFallback(String replacement)
    {
        __replacement = replacement;
    }

    public String DefaultString => __replacement;

    public override String GetReplacement(UInt8[] bytesUnknown, Int32 index) => __replacement;

    private String __replacement;
}

////////// ////////// ////////// ////////// //////////

// The unknown bytes raise a DecoderFallbackException.
public class DecoderExceptionFallback : DecoderFallback
{
    public override String GetReplacement(UInt8[] bytesUnknown, Int32 index)
    {
        throw new DecoderFallbackException(bytesUnknown, index);
    }
}

////////// ////////// ////////// ////////// //////////

// Bytes cannot be decoded, and the fallback is a DecoderExceptionFallback.
public class DecoderFallbackException : ArgumentException
{
    public DecoderFallbackException(UInt8[] bytesUnknown, Int32 index)
        : base("the bytes at the index " + index + " cannot be decoded")
    {
        __bytes = bytesUnknown;
        __index = index;
    }

    // The bytes that cannot be decoded.
    public UInt8[] BytesUnknown => __bytes;

    // The index of the first of them in the array that is decoded.
    public Int32 Index => __index;

    private UInt8[] __bytes;

    private Int32 __index;
}
