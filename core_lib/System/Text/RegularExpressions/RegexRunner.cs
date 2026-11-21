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
internal class RegexRunner
{
    internal RegexRunner(RegexProgram program)
    {
        __program = program;
        __stack = new Int32[96];
        __sp = 0;
        __starts = new Int32[program.GroupCount];
        __registers = new Int32[program.RegisterCount];
        __log = new Int32[48];
        __logLength = 0;
    }

    // The most instructions of a search (a RegexMatchTimeoutException when there are more), 0 when it is not limited.
    internal Int64 MaxSteps;

    // Where the match that was found starts and ends.
    internal Int32 MatchStart;
    internal Int32 MatchEnd;

    // The captures of the groups, in the order they were made: group, start, end.
    internal Int32[] Log => __log;

    // The number of the Int32 of the log that are used.
    internal Int32 LogLength => __logLength;

    // Finds the first match that starts at the position or after it.
    internal Boolean Scan(String text, Int32 startAt)
    {
        __text = text;
        __steps = 0;
        Int32 length = text.Length;
        Int32 first = __program.FirstChar;

        for (Int32 i = startAt; i <= length; i++)
        {
            if (first >= 0)
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
        return index >= 0 && index < __text.Length && RegexCharClass.IsWordChar(__text[index]);
    }

    // Runs the program from the position. The entries of the stack are: a choice (the instruction, the
    // position), or a change that is undone (-1 the start of a group, -2 a register, -3 the length of the log).
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

            if (op == RegexOp.Char)
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
            }
            else if (op == RegexOp.CharIgnoreCase)
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
            }
            else if (op == RegexOp.Any)
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
            }
            else if (op == RegexOp.AnyNl)
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
            }
            else if (op == RegexOp.Set)
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
            }
            else if (op == RegexOp.Split)
            {
                __Push(code[pc + 2], pos, 0);
                pc = code[pc + 1];
            }
            else if (op == RegexOp.Jump)
            {
                pc = code[pc + 1];
            }
            else if (op == RegexOp.Open)
            {
                Int32 g = code[pc + 1];
                __Push(-1, g, __starts[g]);
                __starts[g] = pos;
                pc += 2;
            }
            else if (op == RegexOp.Close)
            {
                Int32 g = code[pc + 1];
                __Push(-3, __logLength, 0);
                __AddLog(g, __starts[g], pos);
                pc += 2;
            }
            else if (op == RegexOp.Mark)
            {
                Int32 r = code[pc + 1];
                __Push(-2, r, __registers[r]);
                __registers[r] = pos;
                pc += 2;
            }
            else if (op == RegexOp.CheckEmpty)
            {
                if (__registers[code[pc + 1]] == pos)
                    pc = code[pc + 2];
                else
                    pc += 3;
            }
            else if (op == RegexOp.Bos)
            {
                if (pos == 0)
                    pc += 1;
                else
                    ok = false;
            }
            else if (op == RegexOp.Bol)
            {
                if (pos == 0 || text[pos - 1] == '\n')
                    pc += 1;
                else
                    ok = false;
            }
            else if (op == RegexOp.Eol)
            {
                if (pos == length || text[pos] == '\n')
                    pc += 1;
                else
                    ok = false;
            }
            else if (op == RegexOp.Eos)
            {
                if (pos == length)
                    pc += 1;
                else
                    ok = false;
            }
            else if (op == RegexOp.EosNl)
            {
                if (pos == length || (pos == length - 1 && text[pos] == '\n'))
                    pc += 1;
                else
                    ok = false;
            }
            else if (op == RegexOp.WordBoundary)
            {
                if (__IsWordAt(pos - 1) != __IsWordAt(pos))
                    pc += 1;
                else
                    ok = false;
            }
            else if (op == RegexOp.NotWordBoundary)
            {
                if (__IsWordAt(pos - 1) == __IsWordAt(pos))
                    pc += 1;
                else
                    ok = false;
            }
            else if (op == RegexOp.BackRef || op == RegexOp.BackRefIgnoreCase)
            {
                if (__LastCapture(code[pc + 1]) && pos + (__refEnd - __refStart) <= length)
                {
                    Int32 n = __refEnd - __refStart;
                    Boolean same = true;
                    for (Int32 i = 0; i < n && same; i++)
                    {
                        Char a = text[__refStart + i];
                        Char b = text[pos + i];
                        same = a == b || (op == RegexOp.BackRefIgnoreCase && Char.ToLower(a) == Char.ToLower(b));
                    }

                    if (same)
                    {
                        pos += n;
                        pc += 2;
                    }
                    else
                    {
                        ok = false;
                    }
                }
                else
                {
                    // A group that did not match: nothing matches it.
                    ok = false;
                }
            }
            else if (op == RegexOp.IfGroup)
            {
                if (__LastCapture(code[pc + 1]))
                    pc += 3;
                else
                    pc = code[pc + 2];
            }
            else if (op == RegexOp.Back)
            {
                if (pos >= code[pc + 1])
                {
                    pos -= code[pc + 1];
                    pc += 2;
                }
                else
                {
                    ok = false;
                }
            }
            else if (op == RegexOp.AtomicStart)
            {
                __Push(-6, pos, 0);
                pc += 1;
            }
            else if (op == RegexOp.AtomicEnd)
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
                    if (__stack[read] < 0)
                    {
                        __stack[write] = __stack[read];
                        __stack[write + 1] = __stack[read + 1];
                        __stack[write + 2] = __stack[read + 2];
                        write += 3;
                    }
                }

                __sp = write;
                if (code[pc + 1] == 1)
                    pos = saved;

                pc += 2;
            }
            else if (op == RegexOp.NotStart)
            {
                __Push(-7, pos, code[pc + 1]);
                pc += 2;
            }
            else if (op == RegexOp.NotEnd)
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
            }
            else
            {
                // Match.
                MatchStart = start;
                MatchEnd = pos;
                return true;
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

    // Finds the last capture of the group that is not undone: __refStart and __refEnd.
    private Boolean __LastCapture(Int32 group)
    {
        for (Int32 i = __logLength - 3; i >= 0; i -= 3)
        {
            if (__log[i] == group)
            {
                __refStart = __log[i + 1];
                __refEnd = __log[i + 2];
                return true;
            }
        }

        return false;
    }

    private Int32 __refStart;
    private Int32 __refEnd;
    private Int64 __steps;
    private RegexProgram __program;
    private String __text;
    private Int32[] __stack;
    private Int32 __sp;
    private Int32[] __starts;
    private Int32[] __registers;
    private Int32[] __log;
    private Int32 __logLength;
}
