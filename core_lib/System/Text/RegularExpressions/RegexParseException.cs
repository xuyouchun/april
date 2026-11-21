/*
 *  System.dll
 *
 *  RegexParseException: a pattern is not a valid regular expression.
 */

namespace Text.RegularExpressions;

////////// ////////// ////////// ////////// //////////

// The pattern of a regular expression is not valid. The message tells what is wrong and where.
public class RegexParseException : ArgumentException
{
    public RegexParseException(String message, String pattern, Int32 offset)
        : base("Invalid pattern '" + pattern + "' at offset " + offset + ": " + message)
    {
        __pattern = pattern;
        __offset = offset;
    }

    // The pattern.
    public String Pattern => __pattern;

    // The index of the character of the pattern where the error was found.
    public Int32 Offset => __offset;

    private String __pattern;
    private Int32 __offset;
}
