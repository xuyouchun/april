/*
 *  System.dll
 *
 *  Capture, Group, Match: the results of a regular expression.
 */

namespace Text.RegularExpressions;

using Collections;

////////// ////////// ////////// ////////// //////////

// A part of the text that a group matched.
public class Capture
{
    internal Capture(String text, Int32 index, Int32 length)
    {
        __text = text;
        __index = index;
        __length = length;
    }

    // The index of the first character.
    public Int32 Index => __index;

    // The number of the characters.
    public Int32 Length => __length;

    // The characters.
    public String Value => __text.Substring(__index, __length);

    public String ToString() => Value;

    // The text that the capture is a part of.
    internal String Source => __text;

    private String __text;
    private Int32 __index;
    private Int32 __length;
}

////////// ////////// ////////// ////////// //////////

// The captures of a group, in the order they were made.
public class CaptureCollection : IReadOnlyList<Capture>
{
    internal CaptureCollection(Capture[] captures)
    {
        __captures = captures;
    }

    public Int32 Count => __captures.Length;

    public Capture this[Int32 index]
    {
        get
        {
            if (index < 0 || index >= __captures.Length)
                throw new OutOfRangeException();

            return __captures[index];
        }
    }

    public IEnumerator<Capture> GetEnumerator() => new ArrayEnumerator<Capture>(__captures);

    private Capture[] __captures;
}

////////// ////////// ////////// ////////// //////////

// A group of a match. A group that did not match is not successful, and has no characters.
public class Group : Capture
{
    internal Group(String text, Int32 index, Int32 length, String name, Capture[] captures)
        : base(text, index, length)
    {
        __name = name;
        __captures = new CaptureCollection(captures);
    }

    // Whether the group matched.
    public Boolean Success => __captures.Count > 0;

    // The name of the group: the number when it has no name.
    public String Name => __name;

    // All the captures of the group (a group in a loop captures once for each turn). The last one is the value.
    public CaptureCollection Captures => __captures;

    private String __name;
    private CaptureCollection __captures;
}

////////// ////////// ////////// ////////// //////////

// The groups of a match, by the number and by the name. Group 0 is the match.
public class GroupCollection : IReadOnlyList<Group>
{
    internal GroupCollection(Regex? regex, Group[] groups)
    {
        __regex = regex;
        __groups = groups;
    }

    public Int32 Count => __groups.Length;

    // The group of the number: a group that is not in the pattern is a group that did not match.
    public Group this[Int32 groupNumber]
    {
        get
        {
            Int32 index = __regex == null ? -1 : __regex.IndexOfNumber(groupNumber);
            if (index < 0)
                return __Failed();

            return __groups[index];
        }
    }

    // The group at the place (the groups are in the order of their numbers).
    internal Group At(Int32 index)
    {
        return __groups[index];
    }

    // The group of the name: a name that is not in the pattern is a group that did not match.
    public Group this[String groupName]
    {
        get
        {
            Int32 index = __regex == null ? -1 : __regex.IndexOfNumber(__regex.GroupNumberFromName(groupName));
            if (index < 0)
                return __Failed();

            return __groups[index];
        }
    }

    public IEnumerator<Group> GetEnumerator() => new ArrayEnumerator<Group>(__groups);

    private Group __Failed() => new Group("", 0, 0, "", new Capture[0]);

    private Regex? __regex;
    private Group[] __groups;
}

////////// ////////// ////////// ////////// //////////

// The result of a search: the part of the text that matched, and its groups. A match that is not successful is
// the result when there is nothing to find.
public class Match : Group
{
    internal Match(Regex? regex, String text, Int32 index, Int32 length, Capture[] captures, Group[] groups, Boolean empty)
        : base(text, index, length, "0", captures)
    {
        __regex = regex;
        __groups = groups;
        __empty = empty;
    }

    // The result of a search that found nothing.
    public static Match Empty => new Match(null, "", 0, 0, new Capture[0], new Group[0], true);

    // The groups. The groups of an unsuccessful match are none.
    public GroupCollection Groups => new GroupCollection(__regex, __groups);

    // The next match in the text, that starts where this one ends. After a match that has no character the
    // search starts at the next character, so that it goes on.
    public Match NextMatch()
    {
        if (__empty)
            return this;

        Int32 start;
        if (__regex.RightToLeft)
        {
            start = this.Index;
            if (this.Length == 0)
                start--;

            if (start < 0)
                return Match.Empty;
        }
        else
        {
            start = this.Index + this.Length;
            if (this.Length == 0)
                start++;

            if (start > this.Source.Length)
                return Match.Empty;
        }

        return __regex.Match(this.Source, start);
    }

    // The text of the replacement with the $ substitutions ($1, ${name}, $&, $`, $', $+, $_, $$) made.
    public String Result(String replacement)
    {
        if (replacement == null)
            throw new ArgumentException("replacement is null");

        if (__empty)
            throw new NotSupportedException("The match is not successful");

        StringBuilder sb = new StringBuilder();
        RegexReplacement.Expand(sb, replacement, this);
        return sb.ToString();
    }

    internal Regex? Owner => __regex;

    private Regex? __regex;
    private Group[] __groups;
    private Boolean __empty;
}
