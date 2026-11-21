/*
 *  System.dll
 *
 *  RegexMatchTimeoutException: a search took more steps than the limit.
 */

namespace Text.RegularExpressions;

////////// ////////// ////////// ////////// //////////

// A search of a regular expression ran more instructions than the limit of the Regex (the library has no clock, so the
// limit is a number of steps, not a time).
public class RegexMatchTimeoutException : SystemException
{
    public RegexMatchTimeoutException(String input, String pattern, Int64 maxSteps)
        : base("The search of the pattern '" + pattern + "' took more than " + maxSteps + " steps")
    {
        __input = input;
        __pattern = pattern;
        __maxSteps = maxSteps;
    }

    // The text that was searched.
    public String Input => __input;

    // The pattern.
    public String Pattern => __pattern;

    // The limit.
    public Int64 MaxSteps => __maxSteps;

    private String __input;
    private String __pattern;
    private Int64 __maxSteps;
}
