/*
 *  System.dll
 *
 *  RegexProgram: the instructions of a regular expression, and the compiler that makes them of a tree.
 */

namespace Text.RegularExpressions;

using Collections;

////////// ////////// ////////// ////////// //////////

// The codes of the instructions. An instruction is its code and the operands, all of them Int32. The instructions that
// read a character have a variant that reads the character before the position (R), for a pattern that is run from
// the right to the left, and for a look behind.
internal static class RegexOp
{
    internal const Int32 Char = 1;          // c: the character
    internal const Int32 CharIgnoreCase = 2; // c: the character in the lower case
    internal const Int32 Any = 3;           // any character but \n
    internal const Int32 AnyNl = 4;         // any character
    internal const Int32 Set = 5;           // i: the index of the set
    internal const Int32 Bos = 6;           // the beginning of the text
    internal const Int32 Bol = 7;           // the beginning of a line
    internal const Int32 Eol = 8;           // the end of a line
    internal const Int32 Eos = 9;           // the end of the text
    internal const Int32 EosNl = 10;        // the end of the text, or before the \n at the end
    internal const Int32 WordBoundary = 11;
    internal const Int32 NotWordBoundary = 12;
    internal const Int32 Open = 13;         // g: the start of a group
    internal const Int32 Close = 14;        // g: the end of a group
    internal const Int32 Split = 15;        // a b: goes to a, and to b when it fails
    internal const Int32 Jump = 16;         // a
    internal const Int32 Mark = 17;         // r: keeps the position in the register
    internal const Int32 CheckEmpty = 18;   // r t: goes to t when the position is the one of the register
    internal const Int32 Match = 19;
    internal const Int32 BackRef = 20;      // g: the text that the group captured last
    internal const Int32 BackRefIgnoreCase = 21;
    internal const Int32 AtomicStart = 22;  // the choices that are made up to AtomicEnd are not tried again
    internal const Int32 AtomicEnd = 23;    // r: goes back to the position of AtomicStart when r is 1 (an assertion)
    internal const Int32 NotStart = 24;     // c: the part up to NotEnd must not match; goes on at c when it does not
    internal const Int32 NotEnd = 25;
    internal const Int32 IfGroup = 27;      // g e: goes to e when the group has not matched
    internal const Int32 CharR = 28;
    internal const Int32 CharIgnoreCaseR = 29;
    internal const Int32 AnyR = 30;
    internal const Int32 AnyNlR = 31;
    internal const Int32 SetR = 32;
    internal const Int32 BackRefR = 33;
    internal const Int32 BackRefIgnoreCaseR = 34;
    internal const Int32 CloseR = 35;       // g: the end of a group that is run from the right: the text is from here to the Open
    internal const Int32 Single = 36;       // kind arg min max flags: a loop of a character (see RegexSingle)
    internal const Int32 StartAt = 37;      // \G: the position where the search started
    internal const Int32 Balance = 38;      // a b r: the balancing group: it cancels the last capture of b, and captures a
}

////////// ////////// ////////// ////////// //////////

// The characters of a loop of one character (Single): the kind of the instruction that reads it, and the flags.
internal static class RegexSingle
{
    internal const Int32 Char = 0;          // arg: the character
    internal const Int32 CharIgnoreCase = 1; // arg: the character in the lower case
    internal const Int32 Any = 2;
    internal const Int32 AnyNl = 3;
    internal const Int32 Set = 4;           // arg: the index of the set

    internal const Int32 Lazy = 1;          // flags: the least is taken first
    internal const Int32 RightToLeft = 2;   // flags: the characters before the position
}

////////// ////////// ////////// ////////// //////////

// A regular expression that is ready to run.
internal class RegexProgram
{
    internal Int32[] Code;

    // The pattern.
    internal String Source;

    internal RegexCharClass[] Sets;

    // The number of the groups (the group 0 too), their names, the number of the registers of the loops.
    internal Int32 GroupCount;
    internal String[] GroupNames;
    internal Int32 RegisterCount;

    // The first character of every match, or -1 when it is not known; the text that every match starts with, or null.
    internal Int32 FirstChar;
    internal String? Prefix;

    // The match can only start at the beginning of the text.
    internal Boolean Anchored;

    // The search goes from the end of the text to the beginning.
    internal Boolean RightToLeft;
}

////////// ////////// ////////// ////////// //////////

// Makes the instructions of a tree. A count of a loop ({n,m}) is made by copies of the part, except the loop of one
// character, that is an instruction.
internal class RegexCompiler
{
    // The most instructions of a program: a pattern like (((a{100}){100}){100}) is not made.
    private const Int32 MaxCode = 1000000;

    internal RegexCompiler(RegexParser parser, RegexOptions options)
    {
        __parser = parser;
        __code = new List<Int32>();
        __sets = new List<RegexCharClass>();
        __registers = 0;
        __rtl = (options & RegexOptions.RightToLeft) != RegexOptions.None;
        __rightToLeft = __rtl;
    }

    internal RegexProgram Compile(RegexNode root)
    {
        __Emit(root);
        __code.Add(RegexOp.Match);

        RegexProgram p = new RegexProgram();
        p.Code = __code.ToArray();
        p.Sets = __sets.ToArray();
        p.GroupCount = __parser.GroupCount;
        p.GroupNames = __parser.GroupNames;
        p.RegisterCount = __registers;
        p.RightToLeft = __rightToLeft;

        p.FirstChar = -1;
        if (!__rightToLeft)
        {
            StringBuilder prefix = new StringBuilder();
            __CollectPrefix(root, prefix);
            if (prefix.Length >= 1)
                p.FirstChar = (Int32)prefix[0];

            if (prefix.Length >= 2)
                p.Prefix = prefix.ToString();

            p.Anchored = p.Code[0] == RegexOp.Bos;
        }

        return p;
    }

    // Collects the characters that every match starts with. Returns false when the part has other things, so that the
    // characters after it are not known.
    private Boolean __CollectPrefix(RegexNode node, StringBuilder prefix)
    {
        switch (node.Kind)
        {
            case RegexNodeKind.Empty:
            case RegexNodeKind.Bos:
            case RegexNodeKind.Bol:
            case RegexNodeKind.WordBoundary:
            case RegexNodeKind.NotWordBoundary:
                return true;

            case RegexNodeKind.Char:
            {
                if (node.IgnoreCase && Char.ToLower((Char)node.Ch) != Char.ToUpper((Char)node.Ch))
                    return false;

                prefix.Append((Char)node.Ch);
                return true;
            }

            case RegexNodeKind.Concat:
            {
                for (Int32 i = 0; i < node.Children.Count; i++)
                {
                    if (!__CollectPrefix(node.Children[i], prefix))
                        return false;
                }

                return true;
            }

            case RegexNodeKind.Group:
            {
                if (node.Balance)
                    return false;

                return __CollectPrefix(node.Children[0], prefix);
            }

            default:
                return false;
        }
    }

    private Int32 __Pc => __code.Count;

    private Void __Emit1(Int32 op)
    {
        __code.Add(op);
    }

    private Void __Emit2(Int32 op, Int32 a)
    {
        __code.Add(op);
        __code.Add(a);
    }

    private Void __Emit3(Int32 op, Int32 a, Int32 b)
    {
        __code.Add(op);
        __code.Add(a);
        __code.Add(b);
    }

    // The code of the instruction that reads a character: the one for the direction.
    private Int32 __Dir(Int32 forward, Int32 backward)
    {
        return __rtl ? backward : forward;
    }

    private Void __Emit(RegexNode node)
    {
        if (__code.Count > MaxCode)
            throw new ArgumentException("The regular expression is too large");

        switch (node.Kind)
        {
            case RegexNodeKind.Empty:
            {
                break;
            }

            case RegexNodeKind.Char:
            {
                Char c = (Char)node.Ch;
                if (node.IgnoreCase && Char.ToLower(c) != Char.ToUpper(c))
                    __Emit2(__Dir(RegexOp.CharIgnoreCase, RegexOp.CharIgnoreCaseR), (Int32)Char.ToLower(c));
                else
                    __Emit2(__Dir(RegexOp.Char, RegexOp.CharR), node.Ch);

                break;
            }

            case RegexNodeKind.Set:
            {
                __Emit2(__Dir(RegexOp.Set, RegexOp.SetR), __sets.Count);
                __sets.Add(node.Set);
                break;
            }

            case RegexNodeKind.Any:
            {
                if (node.DotAll)
                    __Emit1(__Dir(RegexOp.AnyNl, RegexOp.AnyNlR));
                else
                    __Emit1(__Dir(RegexOp.Any, RegexOp.AnyR));

                break;
            }

            case RegexNodeKind.Concat:
            {
                // From the right to the left the parts are run in the opposite order.
                Int32 count = node.Children.Count;
                for (Int32 i = 0; i < count; i++)
                    __Emit(node.Children[__rtl ? count - 1 - i : i]);

                break;
            }

            case RegexNodeKind.Alt:
            {
                __EmitAlt(node);
                break;
            }

            case RegexNodeKind.Group:
            {
                __EmitGroup(node);
                break;
            }

            case RegexNodeKind.Repeat:
            {
                __EmitRepeat(node);
                break;
            }

            case RegexNodeKind.BackRef:
            {
                if (node.IgnoreCase)
                    __Emit2(__Dir(RegexOp.BackRefIgnoreCase, RegexOp.BackRefIgnoreCaseR), node.RefNumber);
                else
                    __Emit2(__Dir(RegexOp.BackRef, RegexOp.BackRefR), node.RefNumber);

                break;
            }

            case RegexNodeKind.Atomic:
            {
                __Emit1(RegexOp.AtomicStart);
                __Emit(node.Children[0]);
                __Emit2(RegexOp.AtomicEnd, 0);
                break;
            }

            case RegexNodeKind.Look:
            {
                __EmitLook(node);
                break;
            }

            case RegexNodeKind.Cond:
            {
                Int32 test = __Pc;
                __Emit3(RegexOp.IfGroup, node.RefNumber, 0);
                __Emit(node.Children[0]);
                Int32 jump = __Pc + 1;
                __Emit2(RegexOp.Jump, 0);
                __code[test + 2] = __Pc;
                __Emit(node.Children[1]);
                __code[jump] = __Pc;
                break;
            }

            case RegexNodeKind.Bos:
            {
                __Emit1(RegexOp.Bos);
                break;
            }

            case RegexNodeKind.Bol:
            {
                __Emit1(RegexOp.Bol);
                break;
            }

            case RegexNodeKind.Eol:
            {
                __Emit1(RegexOp.Eol);
                break;
            }

            case RegexNodeKind.Eos:
            {
                __Emit1(RegexOp.Eos);
                break;
            }

            case RegexNodeKind.EosNl:
            {
                __Emit1(RegexOp.EosNl);
                break;
            }

            case RegexNodeKind.WordBoundary:
            {
                __Emit1(RegexOp.WordBoundary);
                break;
            }

            case RegexNodeKind.NotWordBoundary:
            {
                __Emit1(RegexOp.NotWordBoundary);
                break;
            }

            case RegexNodeKind.StartAt:
            {
                __Emit1(RegexOp.StartAt);
                break;
            }
        }
    }

    // (x), (?<a-b>x), (?<-b>x)
    private Void __EmitGroup(RegexNode node)
    {
        if (node.Balance)
        {
            // The position where the group starts is kept, the capture is made when it ends.
            Int32 register = __registers++;
            __Emit2(RegexOp.Mark, register);
            __Emit(node.Children[0]);
            __Code4(RegexOp.Balance, node.GroupNumber, node.RefNumber, register);
            return;
        }

        __Emit2(RegexOp.Open, node.GroupNumber);
        __Emit(node.Children[0]);
        __Emit2(__Dir(RegexOp.Close, RegexOp.CloseR), node.GroupNumber);
    }

    private Void __Code4(Int32 op, Int32 a, Int32 b, Int32 c)
    {
        __code.Add(op);
        __code.Add(a);
        __code.Add(b);
        __code.Add(c);
    }

    // (?=x) (?!x) (?<=x) (?<!x): the part of a look behind is run from the right to the left, from the position.
    private Void __EmitLook(RegexNode node)
    {
        RegexNode body = node.Children[0];
        Int32 notStart = -1;

        if (node.Negative)
        {
            notStart = __Pc;
            __Emit2(RegexOp.NotStart, 0);
        }
        else
        {
            __Emit1(RegexOp.AtomicStart);
        }

        Boolean saved = __rtl;
        __rtl = node.Behind;
        __Emit(body);
        __rtl = saved;

        if (node.Negative)
        {
            __Emit1(RegexOp.NotEnd);
            __code[notStart + 1] = __Pc;
        }
        else
        {
            __Emit2(RegexOp.AtomicEnd, 1);
        }
    }

    // a|b|c: the first that matches, in the order of the pattern.
    private Void __EmitAlt(RegexNode node)
    {
        List<RegexNode> alts = node.Children;
        List<Int32> jumps = new List<Int32>();

        for (Int32 i = 0; i < alts.Count; i++)
        {
            if (i < alts.Count - 1)
            {
                Int32 split = __Pc;
                __Emit3(RegexOp.Split, split + 3, 0);
                __Emit(alts[i]);
                jumps.Add(__Pc + 1);
                __Emit2(RegexOp.Jump, 0);
                __code[split + 2] = __Pc;
            }
            else
            {
                __Emit(alts[i]);
            }
        }

        for (Int32 i = 0; i < jumps.Count; i++)
            __code[jumps[i]] = __Pc;
    }

    // Whether the part is one character: a character, a set, a dot. Its kind and argument are in the RegexSingle form.
    private Boolean __IsSingle(RegexNode node)
    {
        return node.Kind == RegexNodeKind.Char || node.Kind == RegexNodeKind.Set || node.Kind == RegexNodeKind.Any;
    }

    private Void __EmitSingle(RegexNode body, Int32 min, Int32 max, Boolean lazy)
    {
        Int32 kind = RegexSingle.Any;
        Int32 arg = 0;

        switch (body.Kind)
        {
            case RegexNodeKind.Char:
            {
                Char c = (Char)body.Ch;
                if (body.IgnoreCase && Char.ToLower(c) != Char.ToUpper(c))
                {
                    kind = RegexSingle.CharIgnoreCase;
                    arg = (Int32)Char.ToLower(c);
                }
                else
                {
                    kind = RegexSingle.Char;
                    arg = body.Ch;
                }

                break;
            }

            case RegexNodeKind.Set:
            {
                kind = RegexSingle.Set;
                arg = __sets.Count;
                __sets.Add(body.Set);
                break;
            }

            default:
            {
                kind = body.DotAll ? RegexSingle.AnyNl : RegexSingle.Any;
                break;
            }
        }

        Int32 flags = (lazy ? RegexSingle.Lazy : 0) | (__rtl ? RegexSingle.RightToLeft : 0);
        __code.Add(RegexOp.Single);
        __code.Add(kind);
        __code.Add(arg);
        __code.Add(min);
        __code.Add(max);
        __code.Add(flags);
    }

    private Void __EmitRepeat(RegexNode node)
    {
        RegexNode body = node.Children[0];
        Int32 min = node.Min;
        Int32 max = node.Max;
        Boolean lazy = node.Lazy;

        if (max == 0)
            return;

        if (__IsSingle(body))
        {
            __EmitSingle(body, min, max, lazy);
            return;
        }

        if (max < 0)
        {
            // The copies that are needed, except one: the last one is the one of the loop.
            if (min == 0)
            {
                __EmitStar(body, lazy);
            }
            else
            {
                for (Int32 i = 0; i < min - 1; i++)
                    __Emit(body);

                __EmitPlus(body, lazy);
            }

            return;
        }

        for (Int32 i = 0; i < min; i++)
            __Emit(body);

        // The copies that are not needed: each one is tried when the one before matched.
        List<Int32> exits = new List<Int32>();
        for (Int32 i = min; i < max; i++)
        {
            Int32 split = __Pc;
            if (lazy)
                __Emit3(RegexOp.Split, 0, split + 3);
            else
                __Emit3(RegexOp.Split, split + 3, 0);

            exits.Add(lazy ? split + 1 : split + 2);
            __Emit(body);
        }

        for (Int32 i = 0; i < exits.Count; i++)
            __code[exits[i]] = __Pc;
    }

    // body*
    private Void __EmitStar(RegexNode body, Boolean lazy)
    {
        Boolean canBeEmpty = body.CanBeEmpty();
        Int32 loop = __Pc;
        Int32 exitOperand;

        if (lazy)
        {
            __Emit3(RegexOp.Split, 0, loop + 3);
            exitOperand = loop + 1;
        }
        else
        {
            __Emit3(RegexOp.Split, loop + 3, 0);
            exitOperand = loop + 2;
        }

        Int32 register = -1;
        if (canBeEmpty)
        {
            register = __registers++;
            __Emit2(RegexOp.Mark, register);
        }

        __Emit(body);

        Int32 check = -1;
        if (canBeEmpty)
        {
            check = __Pc + 2;
            __Emit3(RegexOp.CheckEmpty, register, 0);
        }

        __Emit2(RegexOp.Jump, loop);

        __code[exitOperand] = __Pc;
        if (check >= 0)
            __code[check] = __Pc;
    }

    // body+ : the body, then the choice to go on.
    private Void __EmitPlus(RegexNode body, Boolean lazy)
    {
        Boolean canBeEmpty = body.CanBeEmpty();
        Int32 loop = __Pc;

        Int32 register = -1;
        if (canBeEmpty)
        {
            register = __registers++;
            __Emit2(RegexOp.Mark, register);
        }

        __Emit(body);

        Int32 check = -1;
        if (canBeEmpty)
        {
            check = __Pc + 2;
            __Emit3(RegexOp.CheckEmpty, register, 0);
        }

        Int32 split = __Pc;
        Int32 exitOperand;
        if (lazy)
        {
            __Emit3(RegexOp.Split, 0, loop);
            exitOperand = split + 1;
        }
        else
        {
            __Emit3(RegexOp.Split, loop, 0);
            exitOperand = split + 2;
        }

        __code[exitOperand] = __Pc;
        if (check >= 0)
            __code[check] = __Pc;
    }

    private RegexParser __parser;
    private List<Int32> __code;
    private List<RegexCharClass> __sets;
    private Int32 __registers;
    private Boolean __rtl;
    private Boolean __rightToLeft;
}
