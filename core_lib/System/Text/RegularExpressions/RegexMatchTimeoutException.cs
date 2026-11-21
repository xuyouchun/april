/*
 *  System.dll
 *
 *  RegexMatchTimeoutException: a search took more steps than the limit.
 */

namespace Text.RegularExpressions;

////////// ////////// ////////// ////////// //////////

// A search of a regular expression took more time than the match timeout of the Regex (or ran more instructions than its limit
// of steps, that is an extra of this library: MaxSteps).
public class RegexMatchTimeoutException : SystemException
{
    // The search took more time than the timeout.
    public RegexMatchTimeoutException(String input, String pattern, TimeSpan matchTimeout)
        : base("The search of the pattern '" + pattern + "' took more than " + matchTimeout)
    {
        __input = input;
        __pattern = pattern;
        __maxSteps = 0;
        __matchTimeout = matchTimeout;
    }

    // The search ran more instructions than the limit.
    public RegexMatchTimeoutException(String input, String pattern, Int64 maxSteps)
        : base("The search of the pattern '" + pattern + "' took more than " + maxSteps + " steps")
    {
        __input = input;
        __pattern = pattern;
        __maxSteps = maxSteps;
        __matchTimeout = TimeSpan.Zero;
    }

    // The text that was searched.
    public String Input => __input;

    // The pattern.
    public String Pattern => __pattern;

    // The timeout (zero when the limit that was passed is the one of the steps).
    public TimeSpan MatchTimeout => __matchTimeout;

    // The limit of the steps (zero when the limit that was passed is the timeout).
    public Int64 MaxSteps => __maxSteps;

    private String __input;
    private String __pattern;
    private Int64 __maxSteps;
    private TimeSpan __matchTimeout;
}
