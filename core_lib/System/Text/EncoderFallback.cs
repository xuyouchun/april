/*
 *  System.dll
 *
 *  EncoderFallback: what an encoding does with a character that it cannot code.
 */

namespace Text;

////////// ////////// ////////// ////////// //////////

// The way that an encoding handles a character that it cannot code: a surrogate that has no pair (UTF-8,
// UTF-32), a character over the limit of an encoding of one byte (ASCII, Latin1), a pair of surrogates in
// those encodings. An encoding has the replacement of its own (U+FFFD or '?') until a fallback is given by
// Encoding.WithEncoderFallback. A new fallback derives from this class and gives GetReplacement.
public abstract class EncoderFallback
{
    // The text that is coded in place of the unknown one: a character, or the two characters of a pair of
    // surrogates, that begin at the index of the text. An exception is raised when the text cannot be replaced.
    public abstract String GetReplacement(String unknown, Int32 index);

    // The replacement is '?'.
    public static EncoderFallback ReplacementFallback
    {
        get
        {
            if (__replacement == null)
                __replacement = new EncoderReplacementFallback();

            return __replacement;
        }
    }

    // An EncoderFallbackException is raised.
    public static EncoderFallback ExceptionFallback
    {
        get
        {
            if (__exception == null)
                __exception = new EncoderExceptionFallback();

            return __exception;
        }
    }

    private static EncoderFallback? __replacement;

    private static EncoderFallback? __exception;
}

////////// ////////// ////////// ////////// //////////

// The unknown text is replaced by a string ('?' unless another one is given).
public class EncoderReplacementFallback : EncoderFallback
{
    public EncoderReplacementFallback() : this("?")
    {
    }

    // The string must not have a surrogate that has no pair: it would be unknown too.
    public EncoderReplacementFallback(String replacement)
    {
        for (Int32 k = 0; k < replacement.Length; k++)
        {
            if (Char.IsHighSurrogate(replacement[k]) && k + 1 < replacement.Length && Char.IsLowSurrogate(replacement[k + 1]))
            {
                k++;
                continue;
            }

            if (Char.IsSurrogate(replacement[k]))
                throw new ArgumentException();
        }

        __replacement = replacement;
    }

    public String DefaultString => __replacement;

    public override String GetReplacement(String unknown, Int32 index) => __replacement;

    private String __replacement;
}

////////// ////////// ////////// ////////// //////////

// The unknown text raises an EncoderFallbackException.
public class EncoderExceptionFallback : EncoderFallback
{
    public override String GetReplacement(String unknown, Int32 index)
    {
        throw new EncoderFallbackException(unknown, index);
    }
}

////////// ////////// ////////// ////////// //////////

// A character cannot be coded, and the fallback is an EncoderExceptionFallback.
public class EncoderFallbackException : ArgumentException
{
    public EncoderFallbackException(String unknown, Int32 index)
        : base("the text at the index " + index + " cannot be coded")
    {
        __unknown = unknown;
        __index = index;
    }

    // The characters that cannot be coded (one, or the two of a pair of surrogates).
    public String UnknownText => __unknown;

    // The index of the characters in the text that is coded.
    public Int32 Index => __index;

    private String __unknown;

    private Int32 __index;
}
