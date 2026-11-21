/*
 *  System.dll
 *
 *  RegexNode: the tree that the parser makes of a pattern.
 */

namespace Text.RegularExpressions;

using Collections;

////////// ////////// ////////// ////////// //////////

internal enum RegexNodeKind : Int32
{
    Empty, Char, Set, Any, Concat, Alt, Group, Repeat,
    Bos, Bol, Eol, Eos, EosNl, WordBoundary, NotWordBoundary,
    BackRef, Look, Atomic, Cond;
}

////////// ////////// ////////// ////////// //////////

// A part of a pattern. Which fields are used depends on the kind.
internal class RegexNode
{
    internal RegexNode(RegexNodeKind kind)
    {
        Kind = kind;
        Max = -1;
    }

    internal RegexNodeKind Kind;

    // Char: the character; IgnoreCase: it is compared in the lower case.
    internal Int32 Ch;
    internal Boolean IgnoreCase;

    // Set.
    internal RegexCharClass? Set;

    // Any: the character \n too.
    internal Boolean DotAll;

    // Concat, Alt: the parts. Group, Repeat: the only part.
    internal List<RegexNode>? Children;

    // Group: the index in the list of the groups of the parser, then the number of the group.
    internal Int32 GroupIndex;
    internal Int32 GroupNumber;

    // BackRef, Cond: the name of the group (or null when it is a number), the number (known after the groups are
    // numbered), and where it is in the pattern (for the error that the group does not exist).
    internal String? Name;
    internal Int32 RefNumber;
    internal Int32 Pos;

    // Look: the assertion is that the part does not match, or it is a look behind. Atomic and Look have the part
    // as the only child. Cond: the children are the part for yes, and the part for no.
    internal Boolean Negative;
    internal Boolean Behind;

    // Repeat: how many times (Max is -1 when there is no limit), and whether the least is taken first.
    internal Int32 Min;
    internal Int32 Max;
    internal Boolean Lazy;

    internal static RegexNode OfChar(Int32 c, Boolean ignoreCase)
    {
        RegexNode n = new RegexNode(RegexNodeKind.Char);
        n.Ch = c;
        n.IgnoreCase = ignoreCase;
        return n;
    }

    internal static RegexNode OfSet(RegexCharClass set)
    {
        RegexNode n = new RegexNode(RegexNodeKind.Set);
        set.Seal();
        n.Set = set;
        return n;
    }

    internal static RegexNode Of(RegexNodeKind kind, List<RegexNode> children)
    {
        RegexNode n = new RegexNode(kind);
        n.Children = children;
        return n;
    }

    internal static RegexNode Wrap(RegexNodeKind kind, RegexNode child)
    {
        RegexNode n = new RegexNode(kind);
        n.Children = new List<RegexNode>();
        n.Children.Add(child);
        return n;
    }

    // Whether the part can match no character: a loop of it must stop when it does.
    internal Boolean CanBeEmpty()
    {
        switch (Kind)
        {
            case RegexNodeKind.Char:
            case RegexNodeKind.Set:
            case RegexNodeKind.Any:
                return false;

            case RegexNodeKind.Concat:
                for (Int32 i = 0; i < Children.Count; i++)
                {
                    if (!Children[i].CanBeEmpty())
                        return false;
                }

                return true;

            case RegexNodeKind.Alt:
                for (Int32 i = 0; i < Children.Count; i++)
                {
                    if (Children[i].CanBeEmpty())
                        return true;
                }

                return false;

            case RegexNodeKind.Group:
                return Children[0].CanBeEmpty();

            case RegexNodeKind.Repeat:
                return Min == 0 || Children[0].CanBeEmpty();

            default:
                return true;
        }
    }

    // The number of characters that the part matches, or -1 when it is not always the same.
    internal Int32 FixedLength()
    {
        RegexNodeKind kind = Kind;

        if (kind == RegexNodeKind.Char || kind == RegexNodeKind.Set || kind == RegexNodeKind.Any)
            return 1;

        if (kind == RegexNodeKind.Group || kind == RegexNodeKind.Atomic)
            return Children[0].FixedLength();

        if (kind == RegexNodeKind.Concat)
        {
            Int32 total = 0;
            for (Int32 i = 0; i < Children.Count; i++)
            {
                Int32 n = Children[i].FixedLength();
                if (n < 0)
                    return -1;

                total += n;
            }

            return total;
        }

        if (kind == RegexNodeKind.Alt || kind == RegexNodeKind.Cond)
        {
            Int32 first = Children[0].FixedLength();
            for (Int32 i = 1; i < Children.Count; i++)
            {
                if (Children[i].FixedLength() != first)
                    return -1;
            }

            return first;
        }

        if (kind == RegexNodeKind.Repeat)
        {
            Int32 n = Children[0].FixedLength();
            if (n == 0)
                return 0;

            return n > 0 && Min == Max ? n * Min : -1;
        }

        if (kind == RegexNodeKind.BackRef)
            return -1;

        // The anchors and the assertions match no character.
        return 0;
    }
}
