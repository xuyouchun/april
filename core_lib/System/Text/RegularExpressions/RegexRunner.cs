/*
 *  System.dll
 *
 *  RegexRunner: runs the instructions of a regular expression on a text.
 */

namespace Text.RegularExpressions;

////////// ////////// ////////// ////////// //////////

// Runs a program by backtracking: when an instruction does not match, the run goes back to the last choice
// and takes the other way. The choices and the changes that must be undone are kept in an array (not in the
// calls), so that a text of any length can be run.
//
// The entries of the stack have three Int32. The first one tells what it is:
//   0 or more   a choice: the instruction to go on at, the position
//   -1          the start of a group was changed: group, the old value
//   -2          a register was changed: register, the old value
//   -3          the log of the captures got longer: the old length
//   -6          the start of an atomic group or of an assertion: the position
//   -7          the start of (?! or (?<! : the position, the instruction to go on at when the part did not match
//   -16 - 2 * p the choices of a loop of one character, greedy, at the instruction p: the position of the least, the
//               position now (the loop can give back the characters, one by one)
//   -17 - 2 * p the same for a lazy loop: the position now, the number of the characters (the loop can take more)
internal class RegexRunner
{
    internal RegexRunner(RegexProgram program)
    {
        __program = program;
        __stack = new Int32[96];
        __sp = 0;
        __starts = new Int32[program.SlotCount];
        __registers = new Int32[program.RegisterCount];
        __log = new Int32[48];
        __logLength = 0;
    }

    // The most instructions of a search (a RegexMatchTimeoutException when there are more), 0 when it is not limited.
    internal Int64 MaxSteps;

    // Where the match that was found starts and ends.
    internal Int32 MatchStart;
    internal Int32 MatchEnd;

    // The captures of the groups, in the order they were made: group, start, end. A group that is cancelled by a
    // balancing group has an entry (-group - 1, 0, 0): it takes away the last capture that is not taken away.
    internal Int32[] Log => __log;

    // The number of the Int32 of the log that are used.
    internal Int32 LogLength => __logLength;

    // Finds the first match that starts at the position or after it (before it, when the search goes to the left).
    internal Boolean Scan(String text, Int32 startAt)
    {
        __text = text;
        __steps = 0;
        __scanStart = startAt;
        Int32 length = text.Length;

        if (__program.RightToLeft)
        {
            for (Int32 i = startAt; i >= 0; i--)
            {
                if (__Run(i))
                    return true;
            }

            return false;
        }

        Int32 first = __program.FirstChar;
        String? prefix = __program.Prefix;

        for (Int32 i = startAt; i <= length; i++)
        {
            if (prefix != null)
            {
                i = text.IndexOf(prefix, i);
                if (i < 0)
                    return false;
            }
            else if (first >= 0)
            {
                i = text.IndexOf((Char)first, i);
                if (i < 0)
                    return false;
            }

            if (__Run(i))
                return true;

            if (__program.Anchored)
                break;
        }

        return false;
    }

    private Void __Push(Int32 a, Int32 b, Int32 c)
    {
        if (__sp + 3 > __stack.Length)
        {
            Int32[] bigger = new Int32[__stack.Length * 2];
            for (Int32 i = 0; i < __sp; i++)
                bigger[i] = __stack[i];

            __stack = bigger;
        }

        __stack[__sp] = a;
        __stack[__sp + 1] = b;
        __stack[__sp + 2] = c;
        __sp += 3;
    }

    private Void __AddLog(Int32 group, Int32 start, Int32 end)
    {
        if (__logLength + 3 > __log.Length)
        {
            Int32[] bigger = new Int32[__log.Length * 2];
            for (Int32 i = 0; i < __logLength; i++)
                bigger[i] = __log[i];

            __log = bigger;
        }

        __log[__logLength] = group;
        __log[__logLength + 1] = start;
        __log[__logLength + 2] = end;
        __logLength += 3;
    }

    private Boolean __IsWordAt(Int32 index)
    {
        if (index < 0 || index >= __text.Length)
            return false;

        Char c = __text[index];
        if (__program.Ecma)
            return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_';

        return RegexCharClass.IsWordChar(c);
    }

    // Whether the character that is read at the position is the one of a loop of one character (RegexSingle).
    private Boolean __One(Int32 kind, Int32 arg, Int32 pos, Boolean rtl)
    {
        Int32 index = rtl ? pos - 1 : pos;
        if (index < 0 || index >= __text.Length)
            return false;

        Char c = __text[index];
        switch (kind)
        {
            case RegexSingle.Char:
            {
                return (Int32)c == arg;
            }

            case RegexSingle.CharIgnoreCase:
            {
                return (Int32)Char.ToLower(c) == arg;
            }

            case RegexSingle.Any:
            {
                return c != '\n';
            }

            case RegexSingle.AnyNl:
            {
                return true;
            }

            default:
            {
                return __program.Sets[arg].Contains(c);
            }
        }
    }

    // Runs the program from the position.
    private Boolean __Run(Int32 start)
    {
        Int32[] code = __program.Code;
        RegexCharClass[] sets = __program.Sets;
        String text = __text;
        Int32 length = text.Length;

        __sp = 0;
        __logLength = 0;
        for (Int32 i = 0; i < __starts.Length; i++)
            __starts[i] = -1;

        for (Int32 i = 0; i < __registers.Length; i++)
            __registers[i] = -1;

        Int32 pc = 0;
        Int32 pos = start;

        while (true)
        {
            Boolean ok = true;
            Int32 op = code[pc];

            if (MaxSteps > 0 && ++__steps > MaxSteps)
                throw new RegexMatchTimeoutException(text, __program.Source, MaxSteps);

            switch (op)
            {
                case RegexOp.Char:
                {
                    if (pos < length && (Int32)text[pos] == code[pc + 1])
                    {
                        pos++;
                        pc += 2;
                    }
                    else
                    {
                        ok = false;
                    }

                    break;
                }

                case RegexOp.CharR:
                {
                    if (pos > 0 && (Int32)text[pos - 1] == code[pc + 1])
                    {
                        pos--;
                        pc += 2;
                    }
                    else
                    {
                        ok = false;
                    }

                    break;
                }

                case RegexOp.CharIgnoreCase:
                {
                    if (pos < length && (Int32)Char.ToLower(text[pos]) == code[pc + 1])
                    {
                        pos++;
                        pc += 2;
                    }
                    else
                    {
                        ok = false;
                    }

                    break;
                }

                case RegexOp.CharIgnoreCaseR:
                {
                    if (pos > 0 && (Int32)Char.ToLower(text[pos - 1]) == code[pc + 1])
                    {
                        pos--;
                        pc += 2;
                    }
                    else
                    {
                        ok = false;
                    }

                    break;
                }

                case RegexOp.Any:
                {
                    if (pos < length && text[pos] != '\n')
                    {
                        pos++;
                        pc += 1;
                    }
                    else
                    {
                        ok = false;
                    }

                    break;
                }

                case RegexOp.AnyR:
                {
                    if (pos > 0 && text[pos - 1] != '\n')
                    {
                        pos--;
                        pc += 1;
                    }
                    else
                    {
                        ok = false;
                    }

                    break;
                }

                case RegexOp.AnyNl:
                {
                    if (pos < length)
                    {
                        pos++;
                        pc += 1;
                    }
                    else
                    {
                        ok = false;
                    }

                    break;
                }

                case RegexOp.AnyNlR:
                {
                    if (pos > 0)
                    {
                        pos--;
                        pc += 1;
                    }
                    else
                    {
                        ok = false;
                    }

                    break;
                }

                case RegexOp.Set:
                {
                    if (pos < length && sets[code[pc + 1]].Contains(text[pos]))
                    {
                        pos++;
                        pc += 2;
                    }
                    else
                    {
                        ok = false;
                    }

                    break;
                }

                case RegexOp.SetR:
                {
                    if (pos > 0 && sets[code[pc + 1]].Contains(text[pos - 1]))
                    {
                        pos--;
                        pc += 2;
                    }
                    else
                    {
                        ok = false;
                    }

                    break;
                }

                case RegexOp.Single:
                {
                    Int32 kind = code[pc + 1];
                    Int32 arg = code[pc + 2];
                    Int32 min = code[pc + 3];
                    Int32 max = code[pc + 4];
                    Int32 flags = code[pc + 5];
                    Boolean lazy = (flags & RegexSingle.Lazy) != 0;
                    Boolean rtl = (flags & RegexSingle.RightToLeft) != 0;
                    Int32 step = rtl ? -1 : 1;

                    Int32 count = 0;
                    Int32 at = pos;
                    while (count < min && __One(kind, arg, at, rtl))
                    {
                        at += step;
                        count++;
                    }

                    if (count < min)
                    {
                        ok = false;
                        break;
                    }

                    if (lazy)
                    {
                        // The least: the loop takes one more character when the run goes back to here.
                        if (max < 0 || count < max)
                            __Push(-17 - 2 * pc, at, count);
                    }
                    else
                    {
                        // The most: the loop gives the characters back, one by one, when the run goes back to here.
                        Int32 low = at;
                        while ((max < 0 || count < max) && __One(kind, arg, at, rtl))
                        {
                            at += step;
                            count++;
                        }

                        if (at != low)
                            __Push(-16 - 2 * pc, low, at);
                    }

                    pos = at;
                    pc += 6;
                    break;
                }

                case RegexOp.Split:
                {
                    __Push(code[pc + 2], pos, 0);
                    pc = code[pc + 1];
                    break;
                }

                case RegexOp.Jump:
                {
                    pc = code[pc + 1];
                    break;
                }

                case RegexOp.Open:
                {
                    Int32 g = code[pc + 1];
                    __Push(-1, g, __starts[g]);
                    __starts[g] = pos;
                    pc += 2;
                    break;
                }

                case RegexOp.Close:
                {
                    Int32 g = code[pc + 1];
                    __Push(-3, __logLength, 0);
                    __AddLog(g, __starts[g], pos);
                    pc += 2;
                    break;
                }

                case RegexOp.CloseR:
                {
                    Int32 g = code[pc + 1];
                    __Push(-3, __logLength, 0);
                    __AddLog(g, pos, __starts[g]);
                    pc += 2;
                    break;
                }

                case RegexOp.Mark:
                {
                    Int32 r = code[pc + 1];
                    __Push(-2, r, __registers[r]);
                    __registers[r] = pos;
                    pc += 2;
                    break;
                }

                case RegexOp.CheckEmpty:
                {
                    if (__registers[code[pc + 1]] == pos)
                        pc = code[pc + 2];
                    else
                        pc += 3;

                    break;
                }

                case RegexOp.Bos:
                {
                    if (pos == 0)
                        pc += 1;
                    else
                        ok = false;

                    break;
                }

                case RegexOp.Bol:
                {
                    if (pos == 0 || text[pos - 1] == '\n')
                        pc += 1;
                    else
                        ok = false;

                    break;
                }

                case RegexOp.Eol:
                {
                    if (pos == length || text[pos] == '\n')
                        pc += 1;
                    else
                        ok = false;

                    break;
                }

                case RegexOp.Eos:
                {
                    if (pos == length)
                        pc += 1;
                    else
                        ok = false;

                    break;
                }

                case RegexOp.EosNl:
                {
                    if (pos == length || (pos == length - 1 && text[pos] == '\n'))
                        pc += 1;
                    else
                        ok = false;

                    break;
                }

                case RegexOp.WordBoundary:
                {
                    if (__IsWordAt(pos - 1) != __IsWordAt(pos))
                        pc += 1;
                    else
                        ok = false;

                    break;
                }

                case RegexOp.NotWordBoundary:
                {
                    if (__IsWordAt(pos - 1) == __IsWordAt(pos))
                        pc += 1;
                    else
                        ok = false;

                    break;
                }

                case RegexOp.StartAt:
                {
                    if (pos == __scanStart)
                        pc += 1;
                    else
                        ok = false;

                    break;
                }

                case RegexOp.BackRef:
                {
                    if (__program.Ecma && !__LastCapture(code[pc + 1]))
                    {
                        pc += 2;
                        break;
                    }

                    ok = __MatchBackRef(code[pc + 1], pos, false, false);
                    if (ok)
                    {
                        pos += __refEnd - __refStart;
                        pc += 2;
                    }

                    break;
                }

                case RegexOp.BackRefIgnoreCase:
                {
                    if (__program.Ecma && !__LastCapture(code[pc + 1]))
                    {
                        pc += 2;
                        break;
                    }

                    ok = __MatchBackRef(code[pc + 1], pos, true, false);
                    if (ok)
                    {
                        pos += __refEnd - __refStart;
                        pc += 2;
                    }

                    break;
                }

                case RegexOp.BackRefR:
                {
                    if (__program.Ecma && !__LastCapture(code[pc + 1]))
                    {
                        pc += 2;
                        break;
                    }

                    ok = __MatchBackRef(code[pc + 1], pos, false, true);
                    if (ok)
                    {
                        pos -= __refEnd - __refStart;
                        pc += 2;
                    }

                    break;
                }

                case RegexOp.BackRefIgnoreCaseR:
                {
                    if (__program.Ecma && !__LastCapture(code[pc + 1]))
                    {
                        pc += 2;
                        break;
                    }

                    ok = __MatchBackRef(code[pc + 1], pos, true, true);
                    if (ok)
                    {
                        pos -= __refEnd - __refStart;
                        pc += 2;
                    }

                    break;
                }

                case RegexOp.IfGroup:
                {
                    if (__LastCapture(code[pc + 1]))
                        pc += 3;
                    else
                        pc = code[pc + 2];

                    break;
                }

                case RegexOp.Balance:
                {
                    // The group cancels the last capture of the group b, and the group a (when it has a name) gets
                    // the text between the two.
                    Int32 a = code[pc + 1];
                    Int32 b = code[pc + 2];
                    Int32 begin = __registers[code[pc + 3]];
                    Int32 lo = begin < pos ? begin : pos;
                    Int32 hi = begin < pos ? pos : begin;

                    if (!__LastCapture(b))
                    {
                        ok = false;
                        break;
                    }

                    Int32 start2 = __refStart;
                    Int32 end2 = __refEnd;
                    Int32 newStart;
                    Int32 newEnd;
                    if (lo >= end2)
                    {
                        newStart = end2;
                        newEnd = lo;
                    }
                    else if (hi <= start2)
                    {
                        newStart = hi;
                        newEnd = start2;
                    }
                    else
                    {
                        newStart = lo > start2 ? lo : start2;
                        newEnd = hi < end2 ? hi : end2;
                    }

                    __Push(-3, __logLength, 0);
                    __AddLog(-b - 1, 0, 0);
                    if (a >= 0)
                        __AddLog(a, newStart, newEnd);

                    pc += 4;
                    break;
                }

                case RegexOp.AtomicStart:
                {
                    __Push(-6, pos, 0);
                    pc += 1;
                    break;
                }

                case RegexOp.AtomicEnd:
                {
                    // The choices that were made since the start are not tried again: the changes that they
                    // made stay on the stack, so that they are undone when the run goes back before them.
                    Int32 marker = __sp - 3;
                    while (__stack[marker] != -6)
                        marker -= 3;

                    Int32 saved = __stack[marker + 1];
                    Int32 write = marker;
                    for (Int32 read = marker + 3; read < __sp; read += 3)
                    {
                        Int32 tag = __stack[read];
                        if (tag < 0 && tag >= -3)
                        {
                            __stack[write] = tag;
                            __stack[write + 1] = __stack[read + 1];
                            __stack[write + 2] = __stack[read + 2];
                            write += 3;
                        }
                    }

                    __sp = write;
                    if (code[pc + 1] == 1)
                        pos = saved;

                    pc += 2;
                    break;
                }

                case RegexOp.NotStart:
                {
                    __Push(-7, pos, code[pc + 1]);
                    pc += 2;
                    break;
                }

                case RegexOp.NotEnd:
                {
                    // The part matched: the assertion does not. What the part changed is undone.
                    while (__sp > 0)
                    {
                        __sp -= 3;
                        Int32 tag = __stack[__sp];
                        if (tag == -7)
                            break;

                        __Undo(tag, __stack[__sp + 1], __stack[__sp + 2]);
                    }

                    ok = false;
                    break;
                }

                default:
                {
                    // Match.
                    if (__program.RightToLeft)
                    {
                        MatchStart = pos;
                        MatchEnd = start;
                    }
                    else
                    {
                        MatchStart = start;
                        MatchEnd = pos;
                    }

                    return true;
                }
            }

            if (ok)
                continue;

            // Goes back to the last choice, and undoes what was changed after it.
            while (true)
            {
                if (__sp == 0)
                    return false;

                __sp -= 3;
                Int32 tag = __stack[__sp];
                if (tag >= 0)
                {
                    pc = tag;
                    pos = __stack[__sp + 1];
                    break;
                }

                if (tag == -7)
                {
                    // The part of a (?! (?<! did not match: the assertion does.
                    pc = __stack[__sp + 2];
                    pos = __stack[__sp + 1];
                    break;
                }

                if (tag <= -16)
                {
                    Int32 t = -16 - tag;
                    Int32 loop = t / 2;
                    Int32 flags = code[loop + 5];
                    Int32 step = (flags & RegexSingle.RightToLeft) != 0 ? -1 : 1;

                    if (t % 2 == 0)
                    {
                        // A greedy loop gives one character back.
                        Int32 now = __stack[__sp + 2] - step;
                        if (now != __stack[__sp + 1])
                        {
                            __stack[__sp + 2] = now;
                            __sp += 3;
                        }

                        pc = loop + 6;
                        pos = now;
                        break;
                    }

                    // A lazy loop takes one character more, when it can.
                    Int32 here = __stack[__sp + 1];
                    Int32 taken = __stack[__sp + 2];
                    if (!__One(code[loop + 1], code[loop + 2], here, step < 0))
                        continue;

                    here += step;
                    taken++;
                    if (code[loop + 4] < 0 || taken < code[loop + 4])
                    {
                        __stack[__sp + 1] = here;
                        __stack[__sp + 2] = taken;
                        __sp += 3;
                    }

                    pc = loop + 6;
                    pos = here;
                    break;
                }

                __Undo(tag, __stack[__sp + 1], __stack[__sp + 2]);
            }
        }
    }

    // Undoes a change: -1 the start of a group, -2 a register, -3 the length of the log (-6 is the start of an atomic
    // group or of an assertion).
    private Void __Undo(Int32 tag, Int32 a, Int32 b)
    {
        if (tag == -1)
            __starts[a] = b;
        else if (tag == -2)
            __registers[a] = b;
        else if (tag == -3)
            __logLength = a;
    }

    // Whether the text that is read at the position (to the right of it, or to the left) is the same as the last
    // capture of the group (its limits are in __refStart and __refEnd). A group that did not match is not matched by
    // anything.
    private Boolean __MatchBackRef(Int32 group, Int32 pos, Boolean ignoreCase, Boolean rtl)
    {
        if (!__LastCapture(group))
            return false;

        Int32 n = __refEnd - __refStart;
        Int32 at = rtl ? pos - n : pos;
        if (at < 0 || at + n > __text.Length)
            return false;

        for (Int32 i = 0; i < n; i++)
        {
            Char a = __text[__refStart + i];
            Char b = __text[at + i];
            if (a != b && !(ignoreCase && Char.ToLower(a) == Char.ToLower(b)))
                return false;
        }

        return true;
    }

    // Finds the last capture of the group that is not undone, and not cancelled by a balancing group: __refStart and
    // __refEnd.
    private Boolean __LastCapture(Int32 group)
    {
        Int32 cancelled = 0;
        for (Int32 i = __logLength - 3; i >= 0; i -= 3)
        {
            Int32 entry = __log[i];
            if (entry == group)
            {
                if (cancelled == 0)
                {
                    __refStart = __log[i + 1];
                    __refEnd = __log[i + 2];
                    return true;
                }

                cancelled--;
            }
            else if (entry == -group - 1)
            {
                cancelled++;
            }
        }

        return false;
    }

    private Int32 __refStart;
    private Int32 __refEnd;
    private Int64 __steps;
    private Int32 __scanStart;
    private RegexProgram __program;
    private String __text;
    private Int32[] __stack;
    private Int32 __sp;
    private Int32[] __starts;
    private Int32[] __registers;
    private Int32[] __log;
    private Int32 __logLength;
}
