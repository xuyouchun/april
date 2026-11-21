/*
 *  System.dll
 *
 *  Regex: a regular expression.
 */

namespace Text.RegularExpressions;

using Collections;

////////// ////////// ////////// ////////// //////////

// All the matches of a regular expression in a text, from left to right.
public class MatchCollection : IReadOnlyList<Match>
{
    internal MatchCollection(Match[] matches)
    {
        __matches = matches;
    }

    public Int32 Count => __matches.Length;

    public Match this[Int32 index]
    {
        get
        {
            if (index < 0 || index >= __matches.Length)
                throw new OutOfRangeException();

            return __matches[index];
        }
    }

    public IEnumerator<Match> GetEnumerator() => new ArrayEnumerator<Match>(__matches);

    private Match[] __matches;
}

////////// ////////// ////////// ////////// //////////

// A regular expression, with the syntax of .NET. The pattern is read and made into instructions once, by the
// constructor; a Regex can be used many times, and by many threads (a search does not change it).
//
// The static methods that take a pattern keep the 15 patterns that were used last.
public class Regex
{
    public Regex(String pattern) : this(pattern, RegexOptions.None)
    {
    }

    public Regex(String pattern, RegexOptions options) : this(pattern, options, 0)
    {
    }

    // A regular expression whose search stops with a RegexMatchTimeoutException when it ran more than maxSteps
    // instructions (0: no limit). The library has no clock, so this is the limit that stands for the match timeout.
    public Regex(String pattern, RegexOptions options, Int64 maxSteps) : this(pattern, options, maxSteps, new TimeSpan(InfiniteTicks))
    {
    }

    // A regular expression whose search stops with a RegexMatchTimeoutException when it took more than the time: a length of time
    // that is more than zero, or InfiniteMatchTimeout.
    public Regex(String pattern, RegexOptions options, TimeSpan matchTimeout) : this(pattern, options, 0, matchTimeout)
    {
    }

    private Regex(String pattern, RegexOptions options, Int64 maxSteps, TimeSpan matchTimeout)
    {
        if (pattern == null)
            throw new ArgumentException("pattern is null");

        // The timeout: more than zero, and not more than 2147483646 milliseconds (24 days), or infinite.
        if (matchTimeout.Ticks != InfiniteTicks && (matchTimeout.Ticks <= 0 || matchTimeout.Ticks > 21474836460000))
            throw new OutOfRangeException("The match timeout is not more than zero, or infinite");

        // ECMAScript goes with the options of the case, of the lines and Compiled only.
        RegexOptions allowed = RegexOptions.ECMAScript | RegexOptions.IgnoreCase | RegexOptions.Multiline | RegexOptions.Compiled;
        if ((options & RegexOptions.ECMAScript) != RegexOptions.None && (options & allowed) != options)
            throw new OutOfRangeException("RegexOptions.ECMAScript can only be used with IgnoreCase, Multiline and Compiled");

        __pattern = pattern;
        __options = options;
        __maxSteps = maxSteps;
        __matchTimeout = matchTimeout;

        RegexParser parser = new RegexParser(pattern, options);
        RegexNode root = parser.Parse();
        __program = new RegexCompiler(parser, options).Compile(root);
        __program.Source = pattern;
    }

    // The options of the constructor.
    public RegexOptions Options => __options;

    // Whether the search goes from the end of the text to the beginning (RegexOptions.RightToLeft).
    public Boolean RightToLeft => __program.RightToLeft;

    // The most instructions of a search, 0 when it is not limited.
    public Int64 MaxSteps => __maxSteps;

    // The time that a search can take: InfiniteMatchTimeout when it is not limited.
    public TimeSpan MatchTimeout => __matchTimeout;

    // The timeout that is no timeout.
    public static TimeSpan InfiniteMatchTimeout => new TimeSpan(InfiniteTicks);

    // The ticks of InfiniteMatchTimeout: -1 millisecond.
    private const Int64 InfiniteTicks = -10000;

    // The pattern.
    public String ToString() => __pattern;

    ////////// ////////// ////////// ////////// //////////
    // The groups.

    // The names of the groups: the number of a group that has no name. Group 0 is the first.
    public String[] GetGroupNames()
    {
        String[] names = new String[__program.GroupCount];
        for (Int32 i = 0; i < names.Length; i++)
            names[i] = __program.GroupNames[i];

        return names;
    }

    // The numbers of the groups, in order: 0, 1, 2 ... (the numbers that the pattern gives to its groups: (?<5>a) has 0 and 5).
    public Int32[] GetGroupNumbers()
    {
        Int32[] numbers = new Int32[__program.GroupCount];
        for (Int32 i = 0; i < numbers.Length; i++)
            numbers[i] = __program.GroupNumbers[i];

        return numbers;
    }

    // The name of the group of the number, the empty string when there is no group.
    public String GroupNameFromNumber(Int32 groupNumber)
    {
        Int32 index = IndexOfNumber(groupNumber);
        return index < 0 ? "" : __program.GroupNames[index];
    }

    // The place of the group in the groups of a match (the order of the numbers), -1 when there is no such group.
    internal Int32 IndexOfNumber(Int32 groupNumber)
    {
        for (Int32 i = 0; i < __program.GroupCount; i++)
        {
            if (__program.GroupNumbers[i] == groupNumber)
                return i;
        }

        return -1;
    }

    // The number of the group of the name (or of the number written as a text), -1 when there is no group.
    public Int32 GroupNumberFromName(String name)
    {
        if (name == null)
            throw new ArgumentException("name is null");

        for (Int32 i = 0; i < __program.GroupCount; i++)
        {
            if (__program.GroupNames[i] == name)
                return __program.GroupNumbers[i];
        }

        return -1;
    }

    ////////// ////////// ////////// ////////// //////////
    // Searching.

    // Whether the pattern matches somewhere in the text.
    public Boolean IsMatch(String input) => IsMatch(input, -1);

    // Whether the pattern matches somewhere in the text, from the index (-1: the beginning, or the end for a search to
    // the left).
    public Boolean IsMatch(String input, Int32 startAt)
    {
        Int32 start = __Start(input, startAt);
        RegexRunner runner = __NewRunner();
        return runner.Scan(input, start);
    }

    // The first match in the text.
    public Match Match(String input) => Match(input, -1);

    // The first match in the text, from the index. A ^ is the beginning of the text, not the index.
    public Match Match(String input, Int32 startAt)
    {
        Int32 start = __Start(input, startAt);
        RegexRunner runner = __NewRunner();
        if (!runner.Scan(input, start))
            return Match.Empty;

        return __MakeMatch(input, runner);
    }

    // All the matches in the text.
    public MatchCollection Matches(String input) => Matches(input, -1);

    // All the matches in the text, from the index.
    public MatchCollection Matches(String input, Int32 startAt)
    {
        return new MatchCollection(__All(input, __Start(input, startAt), -1));
    }

    // The index where a search starts: the one that is given, or the beginning (the end for a search to the left).
    private Int32 __Start(String input, Int32 startAt)
    {
        if (input == null)
            throw new ArgumentException("input is null");

        if (startAt == -1)
            return __program.RightToLeft ? input.Length : 0;

        if (startAt < 0 || startAt > input.Length)
            throw new OutOfRangeException();

        return startAt;
    }

    // Up to count matches (all of them when count is -1), in the order that they are found.
    private Match[] __All(String input, Int32 startAt, Int32 count)
    {
        List<Match> list = new List<Match>();
        RegexRunner runner = __NewRunner();
        Boolean rtl = __program.RightToLeft;
        Int32 at = startAt;

        while (count < 0 || list.Count < count)
        {
            if (at < 0 || at > input.Length || !runner.Scan(input, at))
                break;

            Match m = __MakeMatch(input, runner);
            list.Add(m);

            if (rtl)
            {
                at = m.Index;
                if (m.Length == 0)
                    at--;
            }
            else
            {
                at = m.Index + m.Length;
                if (m.Length == 0)
                    at++;
            }
        }

        return list.ToArray();
    }

    // Makes the match of the last scan of the runner.
    private Match __MakeMatch(String input, RegexRunner runner)
    {
        Int32 count = __program.GroupCount;
        Int32[] log = runner.Log;
        Int32 logLength = runner.LogLength;

        Capture[] whole = new Capture[1];
        whole[0] = new Capture(input, runner.MatchStart, runner.MatchEnd - runner.MatchStart);

        Group[] groups = new Group[count];
        for (Int32 g = 1; g < count; g++)
        {
            Int32 number = __program.GroupNumbers[g];

            // The captures of the group in the order that they were made: a balancing group takes away the last one.
            List<Capture> list = new List<Capture>();
            for (Int32 i = 0; i < logLength; i += 3)
            {
                if (log[i] == number)
                    list.Add(new Capture(input, log[i + 1], log[i + 2] - log[i + 1]));
                else if (log[i] == -number - 1 && list.Count > 0)
                    list.RemoveAt(list.Count - 1);
            }

            Capture[] captures = list.ToArray();
            Int32 n = captures.Length;
            if (n == 0)
                groups[g] = new Group(input, 0, 0, __program.GroupNames[g], captures);
            else
                groups[g] = new Group(input, captures[n - 1].Index, captures[n - 1].Length, __program.GroupNames[g], captures);
        }

        Match m = new Match(this, input, runner.MatchStart, runner.MatchEnd - runner.MatchStart, whole, groups, false);
        groups[0] = m;
        return m;
    }

    ////////// ////////// ////////// ////////// //////////
    // Replacing and splitting.

    // The text with each match replaced. The replacement can have the substitutions: $1 ${name} $& $` $' $+ $_ $$.
    public String Replace(String input, String replacement) => Replace(input, replacement, -1, -1);

    // The text with the first count matches replaced (all of them when count is -1).
    public String Replace(String input, String replacement, Int32 count) => Replace(input, replacement, count, -1);

    // The text with the first count matches replaced, from the index.
    public String Replace(String input, String replacement, Int32 count, Int32 startAt)
    {
        Int32 start = __Start(input, startAt);
        if (replacement == null)
            throw new ArgumentException("replacement is null");

        Match[] matches = __All(input, start, count);
        String[] texts = new String[matches.Length];
        for (Int32 i = 0; i < matches.Length; i++)
        {
            StringBuilder one = new StringBuilder();
            RegexReplacement.Expand(one, replacement, matches[i]);
            texts[i] = one.ToString();
        }

        return __Join(input, matches, texts);
    }

    // The text with each match replaced by the text that the evaluator gives for it (it is called with the match).
    public String Replace(String input, Delegate<String, Match> evaluator) => Replace(input, evaluator, -1, -1);

    public String Replace(String input, Delegate<String, Match> evaluator, Int32 count) => Replace(input, evaluator, count, -1);

    public String Replace(String input, Delegate<String, Match> evaluator, Int32 count, Int32 startAt)
    {
        Int32 start = __Start(input, startAt);

        Match[] matches = __All(input, start, count);
        String[] texts = new String[matches.Length];
        for (Int32 i = 0; i < matches.Length; i++)
            texts[i] = evaluator(matches[i]);

        return __Join(input, matches, texts);
    }

    // The text with the matches (in the order that they were found) replaced by the texts.
    private String __Join(String input, Match[] matches, String[] texts)
    {
        if (matches.Length == 0)
            return input;

        Boolean rtl = __program.RightToLeft;
        StringBuilder sb = new StringBuilder();
        Int32 prev = 0;
        for (Int32 k = 0; k < matches.Length; k++)
        {
            Int32 i = rtl ? matches.Length - 1 - k : k;
            sb.Append(input, prev, matches[i].Index - prev);
            sb.Append(texts[i]);
            prev = matches[i].Index + matches[i].Length;
        }

        sb.Append(input, prev, input.Length - prev);
        return sb.ToString();
    }

    // The parts of the text between the matches. The text of the groups of a match is in the result too.
    public String[] Split(String input) => Split(input, 0, -1);

    // The most count parts (all when count is 0).
    public String[] Split(String input, Int32 count) => Split(input, count, -1);

    // The most count parts (all when count is 0), from the index. The parts are in the order of the text, for a
    // search to the left too.
    public String[] Split(String input, Int32 count, Int32 startAt)
    {
        Int32 start = __Start(input, startAt);
        if (count < 0)
            throw new OutOfRangeException();

        if (count == 1)
            return new String[] { input };

        Match[] found = __All(input, start, count == 0 ? -1 : count - 1);
        Int32 n = found.Length;
        Match[] matches = new Match[n];
        for (Int32 i = 0; i < n; i++)
            matches[i] = found[__program.RightToLeft ? n - 1 - i : i];

        List<String> parts = new List<String>();
        Int32 prev = 0;
        for (Int32 i = 0; i < n; i++)
        {
            Match m = matches[i];
            parts.Add(input.Substring(prev, m.Index - prev));
            prev = m.Index + m.Length;

            GroupCollection groups = m.Groups;
            for (Int32 g = 1; g < groups.Count; g++)
            {
                Group group = groups.At(g);
                if (group.Success)
                    parts.Add(group.Value);
            }
        }

        parts.Add(input.Substring(prev, input.Length - prev));
        return parts.ToArray();
    }

    ////////// ////////// ////////// ////////// //////////
    // The static methods.

    public static Boolean IsMatch(String input, String pattern) => Get(pattern, RegexOptions.None).IsMatch(input);

    public static Boolean IsMatch(String input, String pattern, RegexOptions options) => Get(pattern, options).IsMatch(input);

    public static Match Match(String input, String pattern) => Get(pattern, RegexOptions.None).Match(input);

    public static Match Match(String input, String pattern, RegexOptions options) => Get(pattern, options).Match(input);

    public static MatchCollection Matches(String input, String pattern) => Get(pattern, RegexOptions.None).Matches(input);

    public static MatchCollection Matches(String input, String pattern, RegexOptions options) => Get(pattern, options).Matches(input);

    public static String Replace(String input, String pattern, String replacement) =>
        Get(pattern, RegexOptions.None).Replace(input, replacement);

    public static String Replace(String input, String pattern, String replacement, RegexOptions options) =>
        Get(pattern, options).Replace(input, replacement);

    public static String Replace(String input, String pattern, Delegate<String, Match> evaluator) =>
        Get(pattern, RegexOptions.None).Replace(input, evaluator);

    public static String Replace(String input, String pattern, Delegate<String, Match> evaluator, RegexOptions options) =>
        Get(pattern, options).Replace(input, evaluator);

    public static String[] Split(String input, String pattern) => Get(pattern, RegexOptions.None).Split(input);

    public static String[] Split(String input, String pattern, RegexOptions options) => Get(pattern, options).Split(input);

    // The same, with a match timeout.
    public static Boolean IsMatch(String input, String pattern, RegexOptions options, TimeSpan matchTimeout) =>
        Get(pattern, options, matchTimeout).IsMatch(input);

    public static Match Match(String input, String pattern, RegexOptions options, TimeSpan matchTimeout) =>
        Get(pattern, options, matchTimeout).Match(input);

    public static MatchCollection Matches(String input, String pattern, RegexOptions options, TimeSpan matchTimeout) =>
        Get(pattern, options, matchTimeout).Matches(input);

    public static String Replace(String input, String pattern, String replacement, RegexOptions options, TimeSpan matchTimeout) =>
        Get(pattern, options, matchTimeout).Replace(input, replacement);

    public static String[] Split(String input, String pattern, RegexOptions options, TimeSpan matchTimeout) =>
        Get(pattern, options, matchTimeout).Split(input);

    // The text with the characters that have a meaning in a pattern (\ * + ? | { [ ( ) ^ $ . # and the spaces)
    // written as characters of the text.
    public static String Escape(String str)
    {
        if (str == null)
            throw new ArgumentException("str is null");

        StringBuilder sb = new StringBuilder();
        for (Int32 i = 0; i < str.Length; i++)
        {
            Char c = str[i];
            if (c == '\\' || c == '*' || c == '+' || c == '?' || c == '|' || c == '{' || c == '[' || c == '(' ||
                c == ')' || c == '^' || c == '$' || c == '.' || c == '#' || c == ' ')
            {
                sb.Append('\\');
                sb.Append(c);
            }
            else if (c == '\n')
            {
                sb.Append("\\n");
            }
            else if (c == '\r')
            {
                sb.Append("\\r");
            }
            else if (c == '\t')
            {
                sb.Append("\\t");
            }
            else if (c == '\f')
            {
                sb.Append("\\f");
            }
            else
            {
                sb.Append(c);
            }
        }

        return sb.ToString();
    }

    // The text without the \ of the escapes that Escape makes (and the other ones of the characters).
    public static String Unescape(String str)
    {
        if (str == null)
            throw new ArgumentException("str is null");

        if (str.IndexOf('\\') < 0)
            return str;

        // The pattern is read as a pattern of characters only.
        StringBuilder sb = new StringBuilder();
        Int32 i = 0;
        while (i < str.Length)
        {
            Char c = str[i];
            if (c != '\\')
            {
                sb.Append(c);
                i++;
                continue;
            }

            i++;
            if (i >= str.Length)
                throw new ArgumentException("Illegal \\ at end of pattern");

            Char e = str[i];
            i++;
            if (e == 'n') sb.Append('\n');
            else if (e == 'r') sb.Append('\r');
            else if (e == 't') sb.Append('\t');
            else if (e == 'f') sb.Append('\f');
            else if (e == 'v') sb.Append('\v');
            else if (e == 'a') sb.Append((Char)7);
            else if (e == 'e') sb.Append((Char)27);
            else sb.Append(e);
        }

        return sb.ToString();
    }

    ////////// ////////// ////////// ////////// //////////

    // The regular expression of the pattern, from the cache.
    private static Regex Get(String pattern, RegexOptions options) => Get(pattern, options, new TimeSpan(InfiniteTicks));

    private static Regex Get(String pattern, RegexOptions options, TimeSpan matchTimeout)
    {
        if (pattern == null)
            throw new ArgumentException("pattern is null");

        String key = (Int32)options + ":" + matchTimeout.Ticks + ":" + pattern;

        if (__cache == null)
        {
            __cache = new Dictionary<String, Regex>();
            __cacheKeys = new List<String>();
        }

        if (__cache.ContainsKey(key))
            return __cache[key];

        Regex regex = new Regex(pattern, options, matchTimeout);
        if (__cacheKeys.Count >= 15)
        {
            __cache.Remove(__cacheKeys[0]);
            __cacheKeys.RemoveAt(0);
        }

        __cache[key] = regex;
        __cacheKeys.Add(key);
        return regex;
    }

    private static Dictionary<String, Regex>? __cache;
    private static List<String>? __cacheKeys;

    private String __pattern;
    private RegexOptions __options;
    private Int64 __maxSteps;
    private TimeSpan __matchTimeout;
    private RegexProgram __program;

    private RegexRunner __NewRunner()
    {
        RegexRunner runner = new RegexRunner(__program);
        runner.MaxSteps = __maxSteps;
        runner.MatchTimeoutTicks = __matchTimeout.Ticks == InfiniteTicks ? 0 : __matchTimeout.Ticks;
        return runner;
    }
}

////////// ////////// ////////// ////////// //////////

// The substitutions of a replacement: $1 $12 ${name} $& $` $' $+ $_ $$.
internal static class RegexReplacement
{
    // Writes the replacement for the match to the builder.
    internal static Void Expand(StringBuilder sb, String replacement, Match match)
    {
        String text = match.Source;
        Int32 i = 0;
        Int32 n = replacement.Length;

        while (i < n)
        {
            Char c = replacement[i];
            if (c != '$' || i + 1 >= n)
            {
                sb.Append(c);
                i++;
                continue;
            }

            Char d = replacement[i + 1];
            if (d == '$')
            {
                sb.Append('$');
                i += 2;
            }
            else if (d == '&')
            {
                sb.Append(match.Value);
                i += 2;
            }
            else if (d == '`')
            {
                sb.Append(text, 0, match.Index);
                i += 2;
            }
            else if (d == '\'')
            {
                sb.Append(text, match.Index + match.Length, text.Length - match.Index - match.Length);
                i += 2;
            }
            else if (d == '_')
            {
                sb.Append(text);
                i += 2;
            }
            else if (d == '+')
            {
                GroupCollection groups = match.Groups;
                sb.Append(groups.At(groups.Count - 1).Value);

                i += 2;
            }
            else if (d >= '0' && d <= '9')
            {
                // The most digits that are the number of a group.
                Int32 end = i + 1;
                while (end < n && replacement[end] >= '0' && replacement[end] <= '9' && end - i <= 9)
                    end++;

                Regex? owner = match.Owner;
                Boolean done = false;
                while (end > i + 1 && !done)
                {
                    Int32 number = 0;
                    for (Int32 k = i + 1; k < end; k++)
                        number = number * 10 + ((Int32)replacement[k] - (Int32)'0');

                    if (owner != null && owner.IndexOfNumber(number) >= 0)
                    {
                        sb.Append(match.Groups[number].Value);
                        done = true;
                    }
                    else
                    {
                        end--;
                    }
                }

                if (done)
                {
                    i = end;
                }
                else
                {
                    sb.Append('$');
                    i++;
                }
            }
            else if (d == '{')
            {
                Int32 close = replacement.IndexOf('}', i + 2);
                Boolean done = false;
                if (close > 0)
                {
                    String name = replacement.Substring(i + 2, close - i - 2);
                    Int32 number = match.Owner == null ? -1 : match.Owner.GroupNumberFromName(name);
                    if (number >= 0)
                    {
                        sb.Append(match.Groups[number].Value);
                        done = true;
                    }
                }

                if (done)
                {
                    i = close + 1;
                }
                else
                {
                    sb.Append('$');
                    i++;
                }
            }
            else
            {
                sb.Append('$');
                i++;
            }
        }
    }
}
