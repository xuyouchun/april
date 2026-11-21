/*
 *  System.dll
 *
 *  RegexProgram: the instructions of a regular expression, and the compiler that makes them of a tree.
 */

namespace Text.RegularExpressions;

using Collections;

////////// ////////// ////////// ////////// //////////

// The codes of the instructions. An instruction is its code and the operands, all of them Int32.
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
    internal const Int32 Back = 26;         // n: goes n characters back
    internal const Int32 IfGroup = 27;      // g e: goes to e when the group has not matched
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

    // The first character of every match, or -1 when it is not known.
    internal Int32 FirstChar;

    // The match can only start at the beginning of the text.
    internal Boolean Anchored;
}

////////// ////////// ////////// ////////// //////////

// Makes the instructions of a tree. A count of a loop ({n,m}) is made by copies of the part.
internal class RegexCompiler
{
    // The most instructions of a program: a pattern like (((a{100}){100}){100}) is not made.
    private const Int32 MaxCode = 1000000;

    internal RegexCompiler(RegexParser parser)
    {
        __parser = parser;
        __code = new List<Int32>();
        __sets = new List<RegexCharClass>();
        __registers = 0;
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

        p.FirstChar = -1;
        if (p.Code[0] == RegexOp.Char)
            p.FirstChar = p.Code[1];

        p.Anchored = p.Code[0] == RegexOp.Bos;
        return p;
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

    private Void __Emit(RegexNode node)
    {
        if (__code.Count > MaxCode)
            throw new ArgumentException("The regular expression is too large");

        RegexNodeKind kind = node.Kind;

        if (kind == RegexNodeKind.Empty)
            return;

        if (kind == RegexNodeKind.Char)
        {
            Char c = (Char)node.Ch;
            if (node.IgnoreCase && Char.ToLower(c) != Char.ToUpper(c))
                __Emit2(RegexOp.CharIgnoreCase, (Int32)Char.ToLower(c));
            else
                __Emit2(RegexOp.Char, node.Ch);

            return;
        }

        if (kind == RegexNodeKind.Set)
        {
            __Emit2(RegexOp.Set, __sets.Count);
            __sets.Add(node.Set);
            return;
        }

        if (kind == RegexNodeKind.Any)
        {
            __Emit1(node.DotAll ? RegexOp.AnyNl : RegexOp.Any);
            return;
        }

        if (kind == RegexNodeKind.Concat)
        {
            for (Int32 i = 0; i < node.Children.Count; i++)
                __Emit(node.Children[i]);

            return;
        }

        if (kind == RegexNodeKind.Alt)
        {
            __EmitAlt(node);
            return;
        }

        if (kind == RegexNodeKind.Group)
        {
            __Emit2(RegexOp.Open, node.GroupNumber);
            __Emit(node.Children[0]);
            __Emit2(RegexOp.Close, node.GroupNumber);
            return;
        }

        if (kind == RegexNodeKind.Repeat)
        {
            __EmitRepeat(node);
            return;
        }

        if (kind == RegexNodeKind.BackRef)
        {
            __Emit2(node.IgnoreCase ? RegexOp.BackRefIgnoreCase : RegexOp.BackRef, node.RefNumber);
            return;
        }

        if (kind == RegexNodeKind.Atomic)
        {
            __Emit1(RegexOp.AtomicStart);
            __Emit(node.Children[0]);
            __Emit2(RegexOp.AtomicEnd, 0);
            return;
        }

        if (kind == RegexNodeKind.Look)
        {
            __EmitLook(node);
            return;
        }

        if (kind == RegexNodeKind.Cond)
        {
            Int32 test = __Pc;
            __Emit3(RegexOp.IfGroup, node.RefNumber, 0);
            __Emit(node.Children[0]);
            Int32 jump = __Pc + 1;
            __Emit2(RegexOp.Jump, 0);
            __code[test + 2] = __Pc;
            __Emit(node.Children[1]);
            __code[jump] = __Pc;
            return;
        }

        if (kind == RegexNodeKind.Bos) __Emit1(RegexOp.Bos);
        else if (kind == RegexNodeKind.Bol) __Emit1(RegexOp.Bol);
        else if (kind == RegexNodeKind.Eol) __Emit1(RegexOp.Eol);
        else if (kind == RegexNodeKind.Eos) __Emit1(RegexOp.Eos);
        else if (kind == RegexNodeKind.EosNl) __Emit1(RegexOp.EosNl);
        else if (kind == RegexNodeKind.WordBoundary) __Emit1(RegexOp.WordBoundary);
        else if (kind == RegexNodeKind.NotWordBoundary) __Emit1(RegexOp.NotWordBoundary);
    }

    // (?=x) (?!x) (?<=x) (?<!x)
    private Void __EmitLook(RegexNode node)
    {
        RegexNode body = node.Children[0];
        Int32 length = node.Behind ? body.FixedLength() : 0;
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

        if (node.Behind)
            __Emit2(RegexOp.Back, length);

        __Emit(body);

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

    private Void __EmitRepeat(RegexNode node)
    {
        RegexNode body = node.Children[0];
        Int32 min = node.Min;
        Int32 max = node.Max;
        Boolean lazy = node.Lazy;

        if (max == 0)
            return;

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
}
