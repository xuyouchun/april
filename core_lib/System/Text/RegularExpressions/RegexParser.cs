/*
 *  System.dll
 *
 *  RegexParser: reads a pattern and makes the tree of its parts.
 */

namespace Text.RegularExpressions;

using Collections;

////////// ////////// ////////// ////////// //////////

// Reads a pattern. The syntax is the one of .NET; the blocks of \p{..} are those of RegexUnicode (the basic plane).
internal class RegexParser
{
    internal RegexParser(String pattern, RegexOptions options)
    {
        __pattern = pattern;
        __length = pattern.Length;
        __pos = 0;
        __groups = new List<RegexNode>();
        __names = new List<String?>();
        __refs = new List<RegexNode>();
        __explicitNumbers = new List<Int32>();
        __defined = null;
        __nameANumber = -1;

        __ignoreCase = (options & RegexOptions.IgnoreCase) != RegexOptions.None;
        __multiline = (options & RegexOptions.Multiline) != RegexOptions.None;
        __explicit = (options & RegexOptions.ExplicitCapture) != RegexOptions.None;
        __singleline = (options & RegexOptions.Singleline) != RegexOptions.None;
        __extended = (options & RegexOptions.IgnorePatternWhitespace) != RegexOptions.None;
        __ecma = (options & RegexOptions.ECMAScript) != RegexOptions.None;
        __explicitAtStart = __explicit;
    }

    // The number of the groups, group 0 (the whole match) too. Known after Parse.
    internal Int32 GroupCount;

    // The numbers of the groups, in the order of the numbers: 0 and the others. A pattern can leave numbers out:
    // (?<5>a) has the groups 0 and 5.
    internal Int32[] GroupNumbers;

    // The room that the groups need: the greatest number and one.
    internal Int32 SlotCount;

    // The name of each group (in the order of GroupNumbers): a group that has no name has its number as the name.
    internal String[] GroupNames;

    // The parts of the pattern.
    internal RegexNode Parse()
    {
        RegexNode node = __ParseAlt();

        if (__pos < __length)
            throw __Error("Too many ')' characters");

        __Number();
        __ResolveRefs();
        return node;
    }

    // Numbers the groups: the groups that have a number in their name keep it, then the groups that have no name take the
    // numbers that are not used, from left to right, then the named ones (the groups of one name have one number).
    private Void __Number()
    {
        List<Int32> used = new List<Int32>();
        for (Int32 i = 0; i < __groups.Count; i++)
        {
            Int32 e = __explicitNumbers[i];
            if (e >= 0)
            {
                __groups[i].GroupNumber = e;
                if (!__Has(used, e))
                    used.Add(e);
            }
        }

        Int32 next = 1;
        for (Int32 i = 0; i < __groups.Count; i++)
        {
            if (__names[i] == null && __explicitNumbers[i] < 0)
            {
                while (__Has(used, next))
                    next++;

                __groups[i].GroupNumber = next;
                used.Add(next);
                next++;
            }
        }

        List<String> namedNames = new List<String>();
        List<Int32> namedNumbers = new List<Int32>();
        for (Int32 i = 0; i < __groups.Count; i++)
        {
            String? name = __names[i];
            if (name == null)
                continue;

            Int32 found = -1;
            for (Int32 k = 0; k < namedNames.Count; k++)
            {
                if (namedNames[k] == name)
                    found = k;
            }

            if (found >= 0)
            {
                __groups[i].GroupNumber = namedNumbers[found];
                continue;
            }

            while (__Has(used, next))
                next++;

            __groups[i].GroupNumber = next;
            used.Add(next);
            namedNames.Add(name);
            namedNumbers.Add(next);
            next++;
        }

        // The numbers in order, and the names that go with them.
        Int32 count = used.Count + 1;
        Int32[] numbers = new Int32[count];
        numbers[0] = 0;
        for (Int32 i = 1; i < count; i++)
        {
            Int32 value = used[i - 1];
            Int32 j = i - 1;
            while (j >= 1 && numbers[j] > value)
            {
                numbers[j + 1] = numbers[j];
                j--;
            }

            numbers[j + 1] = value;
        }

        String[] names = new String[count];
        for (Int32 i = 0; i < count; i++)
        {
            names[i] = numbers[i].ToString();
            for (Int32 k = 0; k < namedNumbers.Count; k++)
            {
                if (namedNumbers[k] == numbers[i])
                    names[i] = namedNames[k];
            }
        }

        GroupCount = count;
        GroupNumbers = numbers;
        GroupNames = names;
        SlotCount = numbers[count - 1] + 1;
    }

    private Boolean __Has(List<Int32> list, Int32 value)
    {
        for (Int32 i = 0; i < list.Count; i++)
        {
            if (list[i] == value)
                return true;
        }

        return false;
    }

    // The references to a group (\1, \k<name>, (?(1)..), (?<a-b>..)) are numbers of groups that exist.
    private Void __ResolveRefs()
    {
        for (Int32 i = 0; i < __refs.Count; i++)
        {
            RegexNode node = __refs[i];
            if (node.Name != null)
            {
                Int32 found = -1;
                for (Int32 k = 0; k < GroupCount; k++)
                {
                    if (GroupNames[k] == node.Name)
                        found = GroupNumbers[k];
                }

                if (found < 0)
                    throw new RegexParseException("Reference to undefined group name '" + node.Name + "'", __pattern, node.Pos);

                node.RefNumber = found;
            }
            else
            {
                Boolean exists = false;
                for (Int32 k = 0; k < GroupCount; k++)
                {
                    if (GroupNumbers[k] == node.RefNumber)
                        exists = true;
                }

                if (!exists)
                    throw new RegexParseException("Reference to undefined group number " + node.RefNumber, __pattern, node.Pos);
            }
        }
    }

    // The numbers of the groups of the pattern, group 0 too, found before the pattern is read: a \10 is a reference
    // when there is a group 10, an octal character when there is not. They are numbered as __Number does.
    private List<Int32> __DefinedNumbers()
    {
        if (__defined != null)
            return __defined;

        List<Int32> used = new List<Int32>();
        used.Add(0);
        List<String> named = new List<String>();
        Int32 unnamed = 0;
        Int32 i = 0;
        while (i < __length)
        {
            Char c = __pattern[i];
            if (c == '\\')
            {
                i += 2;
            }
            else if (c == '[')
            {
                i++;
                if (i < __length && __pattern[i] == '^')
                    i++;

                if (i < __length && __pattern[i] == ']')
                    i++;

                Int32 depth = 1;
                while (i < __length && depth > 0)
                {
                    Char d = __pattern[i];
                    if (d == '\\')
                    {
                        i += 2;
                    }
                    else if (d == '-' && i + 1 < __length && __pattern[i + 1] == '[')
                    {
                        depth++;
                        i += 2;
                    }
                    else
                    {
                        if (d == ']')
                            depth--;

                        i++;
                    }
                }
            }
            else if (c == '(')
            {
                if (i + 1 < __length && __pattern[i + 1] == '?')
                {
                    if (i + 3 < __length && (__pattern[i + 2] == '<' || __pattern[i + 2] == '\'')
                            && __pattern[i + 3] != '=' && __pattern[i + 3] != '!')
                    {
                        Char close = __pattern[i + 2] == '<' ? '>' : '\'';
                        Int32 end = i + 3;
                        while (end < __length && __pattern[end] != close)
                            end++;

                        String text = __pattern.Substring(i + 3, end - (i + 3));
                        Int32 dash = text.IndexOf('-');
                        String first = dash < 0 ? text : text.Substring(0, dash);
                        if (first.Length > 0)
                        {
                            if (__AllDigits(first))
                            {
                                Int32 n = __NumberOfName(first);
                                if (n > 0 && !__Has(used, n))
                                    used.Add(n);
                            }
                            else
                            {
                                Boolean seen = false;
                                for (Int32 k = 0; k < named.Count; k++)
                                {
                                    if (named[k] == first)
                                        seen = true;
                                }

                                if (!seen)
                                    named.Add(first);
                            }
                        }
                    }
                }
                else if (!__explicitAtStart)
                {
                    unnamed++;
                }

                i++;
            }
            else
            {
                i++;
            }
        }

        Int32 next = 1;
        for (Int32 k = 0; k < unnamed + named.Count; k++)
        {
            while (__Has(used, next))
                next++;

            used.Add(next);
            next++;
        }

        __defined = used;
        return used;
    }

    // Whether the text is all digits (and not empty).
    private Boolean __AllDigits(String text)
    {
        if (text.Length == 0)
            return false;

        for (Int32 i = 0; i < text.Length; i++)
        {
            if (!Char.IsDigit(text[i]))
                return false;
        }

        return true;
    }

    ////////// ////////// ////////// ////////// //////////

    private RegexParseException __Error(String message)
    {
        return new RegexParseException(message, __pattern, __pos);
    }

    private Boolean __More => __pos < __length;

    private Char __Cur => __pattern[__pos];

    // Skips the spaces and the comments of the pattern when the option says so.
    private Void __SkipSpace()
    {
        if (!__extended)
            return;

        while (__pos < __length)
        {
            Char c = __pattern[__pos];
            if (Char.IsWhiteSpace(c))
            {
                __pos++;
            }
            else if (c == '#')
            {
                while (__pos < __length && __pattern[__pos] != '\n')
                    __pos++;
            }
            else
            {
                break;
            }
        }
    }

    private RegexNode __ParseAlt()
    {
        List<RegexNode> alts = new List<RegexNode>();
        alts.Add(__ParseConcat());

        while (__More && __Cur == '|')
        {
            __pos++;
            alts.Add(__ParseConcat());
        }

        if (alts.Count == 1)
            return alts[0];

        return RegexNode.Of(RegexNodeKind.Alt, alts);
    }

    private RegexNode __ParseConcat()
    {
        List<RegexNode> items = new List<RegexNode>();

        while (true)
        {
            __SkipSpace();
            if (!__More || __Cur == '|' || __Cur == ')')
                break;

            RegexNode? atom = __ParseAtom();
            if (atom == null)
                continue;

            items.Add(__ParseQuantifier(atom));
        }

        if (items.Count == 0)
            return new RegexNode(RegexNodeKind.Empty);

        if (items.Count == 1)
            return items[0];

        return RegexNode.Of(RegexNodeKind.Concat, items);
    }

    ////////// ////////// ////////// ////////// //////////

    // A quantifier after the atom: * + ? {n} {n,} {n,m}, and ? after it for the least.
    private RegexNode __ParseQuantifier(RegexNode atom)
    {
        __SkipSpace();
        if (!__More)
            return atom;

        Int32 min;
        Int32 max;
        Char c = __Cur;
        if (c == '*')
        {
            min = 0;
            max = -1;
            __pos++;
        }
        else if (c == '+')
        {
            min = 1;
            max = -1;
            __pos++;
        }
        else if (c == '?')
        {
            min = 0;
            max = 1;
            __pos++;
        }
        else if (c == '{' && __TryBraces())
        {
            min = __min;
            max = __max;
            if (max >= 0 && min > max)
                throw __Error("Illegal {x,y} with x > y");
        }
        else
        {
            return atom;
        }

        RegexNode r = RegexNode.Wrap(RegexNodeKind.Repeat, atom);
        r.Min = min;
        r.Max = max;

        if (__More && __Cur == '?')
        {
            r.Lazy = true;
            __pos++;
        }

        __SkipSpace();
        if (__More)
        {
            Char next = __Cur;
            if (next == '*' || next == '+' || next == '?')
                throw __Error("Nested quantifier '" + next + "'");

            if (next == '{')
            {
                Int32 save = __pos;
                Boolean braces = __TryBraces();
                __pos = save;
                if (braces)
                    throw __Error("Nested quantifier '{'");
            }
        }

        return r;
    }

    // Reads {n}, {n,} or {n,m} at the current position into __min and __max. When there is something else
    // the position does not change and the result is false: the { is a character of the pattern.
    private Boolean __TryBraces()
    {
        Int32 save = __pos;
        __pos++;

        Int32 min = __ReadNumber();
        if (min < 0)
        {
            __pos = save;
            return false;
        }

        Int32 max = min;
        if (__More && __Cur == ',')
        {
            __pos++;
            if (__More && __Cur == '}')
            {
                max = -1;
            }
            else
            {
                max = __ReadNumber();
                if (max < 0)
                {
                    __pos = save;
                    return false;
                }
            }
        }

        if (!__More || __Cur != '}')
        {
            __pos = save;
            return false;
        }

        __pos++;
        __min = min;
        __max = max;
        return true;
    }

    // Reads the digits of a number, -1 when there are none.
    private Int32 __ReadNumber()
    {
        Int32 start = __pos;
        Int32 n = 0;
        while (__More && __Cur >= '0' && __Cur <= '9')
        {
            n = n * 10 + ((Int32)__Cur - (Int32)'0');
            if (n > 100000)
                throw __Error("The count of a quantifier is too large");

            __pos++;
        }

        return __pos == start ? -1 : n;
    }

    ////////// ////////// ////////// ////////// //////////

    // One part of the pattern. Null for the parts that match nothing and are not quantified: a comment and
    // the options of a group.
    private RegexNode? __ParseAtom()
    {
        Char c = __Cur;

        if (c == '(')
            return __ParseGroup();

        if (c == '[')
            return __ParseClass();

        if (c == '\\')
            return __ParseEscape();

        if (c == '*' || c == '+' || c == '?')
            throw __Error("Quantifier '" + c + "' following nothing");

        __pos++;

        if (c == '.')
        {
            RegexNode any = new RegexNode(RegexNodeKind.Any);
            any.DotAll = __singleline;
            return any;
        }

        if (c == '^')
            return new RegexNode(__multiline ? RegexNodeKind.Bol : RegexNodeKind.Bos);

        if (c == '$')
            return new RegexNode(__multiline ? RegexNodeKind.Eol : RegexNodeKind.EosNl);

        return RegexNode.OfChar((Int32)c, __ignoreCase);
    }

    private RegexNode __OfCategory(Int32 category)
    {
        RegexCharClass set = new RegexCharClass();
        __AddCategory(set, category);
        return RegexNode.OfSet(set);
    }

    // Adds \d \w \s (or the others) to the set. With RegexOptions.ECMAScript they are the characters of ASCII only:
    // \d is 0-9, \w is a-z A-Z 0-9 _, \s is the space and \t \n \v \f \r; the others are the characters that are left.
    private Void __AddCategory(RegexCharClass set, Int32 category)
    {
        if (!__ecma)
        {
            set.AddCategory(category);
            return;
        }

        switch (category)
        {
            case RegexCharClass.Digit:
            {
                set.AddRange(48, 57);
                break;
            }

            case RegexCharClass.NotDigit:
            {
                set.AddRange(0, 47);
                set.AddRange(58, 65535);
                break;
            }

            case RegexCharClass.Word:
            {
                set.AddRange(48, 57);
                set.AddRange(65, 90);
                set.AddRange(95, 95);
                set.AddRange(97, 122);
                break;
            }

            case RegexCharClass.NotWord:
            {
                set.AddRange(0, 47);
                set.AddRange(58, 64);
                set.AddRange(91, 94);
                set.AddRange(96, 96);
                set.AddRange(123, 65535);
                break;
            }

            case RegexCharClass.Space:
            {
                set.AddRange(9, 13);
                set.AddRange(32, 32);
                break;
            }

            default:
            {
                set.AddRange(0, 8);
                set.AddRange(14, 31);
                set.AddRange(33, 65535);
                break;
            }
        }
    }

    // A \ outside a character class.
    private RegexNode __ParseEscape()
    {
        __pos++;
        if (!__More)
            throw __Error("Illegal \\ at end of pattern");

        Char c = __Cur;
        __pos++;

        if (c == 'd') return __OfCategory(RegexCharClass.Digit);
        if (c == 'D') return __OfCategory(RegexCharClass.NotDigit);
        if (c == 'w') return __OfCategory(RegexCharClass.Word);
        if (c == 'W') return __OfCategory(RegexCharClass.NotWord);
        if (c == 's') return __OfCategory(RegexCharClass.Space);
        if (c == 'S') return __OfCategory(RegexCharClass.NotSpace);
        if (c == 'b') return new RegexNode(RegexNodeKind.WordBoundary);
        if (c == 'B') return new RegexNode(RegexNodeKind.NotWordBoundary);
        if (c == 'A') return new RegexNode(RegexNodeKind.Bos);
        if (c == 'z') return new RegexNode(RegexNodeKind.Eos);
        if (c == 'Z') return new RegexNode(RegexNodeKind.EosNl);

        if (c == 'G')
            return new RegexNode(RegexNodeKind.StartAt);

        if (c == 'p' || c == 'P')
        {
            RegexCharClass property = new RegexCharClass();
            __AddProperty(property, c == 'P');
            return RegexNode.OfSet(property);
        }

        if (c == 'k')
        {
            Int32 at = __pos - 2;
            if (!__More || (__Cur != '<' && __Cur != '\''))
                throw __Error("Malformed \\k<...> reference");

            Char close = __Cur == '<' ? '>' : '\'';
            __pos++;
            RegexNode named = new RegexNode(RegexNodeKind.BackRef);
            named.Name = __ReadRefName(close);
            if (Char.IsDigit(named.Name[0]))
            {
                named.RefNumber = __NumberOfName(named.Name);
                named.Name = null;
            }

            named.IgnoreCase = __ignoreCase;
            named.Pos = at;
            __refs.Add(named);
            return named;
        }

        if (c >= '1' && c <= '9')
        {
            Int32 first = __pos - 1;
            Int32 end = __pos;
            while (end < __length && __pattern[end] >= '0' && __pattern[end] <= '9' && end - first < 6)
                end++;

            if (__ecma)
            {
                // The most digits that are the number of a group; when there is none it is an octal character, and
                // 8 and 9 are themselves.
                Int32 best = -1;
                Int32 bestEnd = first;
                Int32 value = 0;
                for (Int32 e = first; e < end; e++)
                {
                    value = value * 10 + ((Int32)__pattern[e] - (Int32)'0');
                    if (__Has(__DefinedNumbers(), value))
                    {
                        best = value;
                        bestEnd = e + 1;
                    }
                }

                if (best >= 0)
                {
                    RegexNode ecmaRef = new RegexNode(RegexNodeKind.BackRef);
                    ecmaRef.RefNumber = best;
                    ecmaRef.IgnoreCase = __ignoreCase;
                    ecmaRef.Pos = first - 1;
                    __refs.Add(ecmaRef);
                    __pos = bestEnd;
                    return ecmaRef;
                }

                if (c > '7')
                    return RegexNode.OfChar((Int32)c, __ignoreCase);

                Int32 octal = 0;
                __pos = first;
                Int32 count = 0;
                while (count < 3 && __More && __Cur >= '0' && __Cur <= '7')
                {
                    octal = octal * 8 + ((Int32)__Cur - (Int32)'0');
                    __pos++;
                    count++;
                }

                return RegexNode.OfChar(octal, __ignoreCase);
            }

            Int32 n = __NumberOfName(__pattern.Substring(first, end - first));
            if (__Has(__DefinedNumbers(), n))
            {
                RegexNode numbered = new RegexNode(RegexNodeKind.BackRef);
                numbered.RefNumber = n;
                numbered.IgnoreCase = __ignoreCase;
                numbered.Pos = first - 1;
                __refs.Add(numbered);
                __pos = end;
                return numbered;
            }

            if (end - first == 1 || c > '7')
                throw new RegexParseException("Reference to undefined group number " + n, __pattern, first - 1);

            // Not a group: an octal character, of up to three digits.
            Int32 v = 0;
            __pos = first;
            Int32 digits = 0;
            while (digits < 3 && __More && __Cur >= '0' && __Cur <= '7')
            {
                v = v * 8 + ((Int32)__Cur - (Int32)'0');
                __pos++;
                digits++;
            }

            return RegexNode.OfChar(v, __ignoreCase);
        }

        Int32 v = __CharEscape(c);
        return RegexNode.OfChar(v, __ignoreCase);
    }

    // The character of an escape (the letter after the \ is read already): \n \t \x41 A \0 \cA, and the
    // punctuation characters that stand for themselves.
    private Int32 __CharEscape(Char c)
    {
        if (c == 'n') return 10;
        if (c == 'r') return 13;
        if (c == 't') return 9;
        if (c == 'f') return 12;
        if (c == 'v') return 11;
        if (c == 'e') return 27;
        if (c == 'a') return 7;
        if (c == 'x') return __ReadHex(2);
        if (c == 'u') return __ReadHex(4);

        if (c == '0')
        {
            Int32 v = 0;
            Int32 digits = 0;
            while (digits < 2 && __More && __Cur >= '0' && __Cur <= '7')
            {
                v = v * 8 + ((Int32)__Cur - (Int32)'0');
                __pos++;
                digits++;
            }

            return v;
        }

        if (c == 'c')
        {
            if (!__More)
                throw __Error("Missing control character");

            Char k = Char.ToUpper(__Cur);
            if (k < '@' || k > '_')
                throw __Error("Unrecognized control character");

            __pos++;
            return (Int32)k - 64;
        }

        if (Char.IsLetterOrDigit(c) || c == '_')
            throw __Error("Unrecognized escape sequence \\" + c);

        return (Int32)c;
    }

    // Reads the exact number of hexadecimal digits.
    private Int32 __ReadHex(Int32 digits)
    {
        Int32 v = 0;
        for (Int32 i = 0; i < digits; i++)
        {
            if (!__More)
                throw __Error("Insufficient hexadecimal digits");

            Char c = __Cur;
            Int32 d;
            if (c >= '0' && c <= '9')
                d = (Int32)c - (Int32)'0';
            else if (c >= 'a' && c <= 'f')
                d = (Int32)c - (Int32)'a' + 10;
            else if (c >= 'A' && c <= 'F')
                d = (Int32)c - (Int32)'A' + 10;
            else
                throw __Error("Insufficient hexadecimal digits");

            v = v * 16 + d;
            __pos++;
        }

        return v;
    }

    ////////// ////////// ////////// ////////// //////////

    // The property of a \\p{name} or \\P{name} (the position is after the p): a category of Unicode (L, Lu, Nd ...) or a
    // block (IsGreek ...). It is added to the set; \\P is the characters that do not have it.
    private Void __AddProperty(RegexCharClass set, Boolean negated)
    {
        if (__ecma)
            throw __Error("The properties of Unicode (\\p) are not supported with RegexOptions.ECMAScript");

        if (!__More || __Cur != '{')
            throw __Error("Incomplete \\p{X} character escape");

        __pos++;
        Int32 start = __pos;
        while (__More && __Cur != '}')
        {
            Char c = __Cur;
            if (!(Char.IsLetterOrDigit(c) || c == '-' || c == '_'))
                throw __Error("Incomplete \\p{X} character escape");

            __pos++;
        }

        if (!__More)
            throw __Error("Incomplete \\p{X} character escape");

        String name = __pattern.Substring(start, __pos - start);
        __pos++;

        Int32 mask = RegexUnicode.CategoryMask(name);
        if (mask != 0)
        {
            set.AddUnicode(mask, negated);
            return;
        }

        Int32 block = RegexUnicode.FindBlock(name);
        if (block < 0)
            throw new RegexParseException("Unknown property '" + name + "'", __pattern, start);

        Int32 first = RegexUnicode.BlockStart(block);
        Int32 last = RegexUnicode.BlockEnd(block);
        if (!negated)
        {
            set.AddRange(first, last);
        }
        else
        {
            if (first > 0)
                set.AddRange(0, first - 1);

            if (last < 0xFFFF)
                set.AddRange(last + 1, 0xFFFF);
        }
    }

    // A character class: [abc] [^abc] [a-z] [\d_] [a-z-[aeiou]].
    private RegexNode __ParseClass()
    {
        RegexCharClass set = __ReadClass();
        return RegexNode.OfSet(set);
    }

    // Reads a class from the [ to the ].
    private RegexCharClass __ReadClass()
    {
        __pos++;

        RegexCharClass set = new RegexCharClass();
        set.IgnoreCase = __ignoreCase;
        if (__More && __Cur == '^')
        {
            set.Negated = true;
            __pos++;
        }

        Boolean first = true;
        while (true)
        {
            if (!__More)
                throw __Error("Unterminated [] set");

            Char c = __Cur;
            if (c == ']' && !first)
            {
                __pos++;
                break;
            }

            // A subtraction is the last part of the class.
            if (c == '-' && !first && __pos + 1 < __length && __pattern[__pos + 1] == '[')
            {
                __pos++;
                set.Subtract = __ReadClass();
                if (!__More || __Cur != ']')
                    throw __Error("A subtraction must be the last part of a character class");

                __pos++;
                break;
            }

            first = false;

            Int32 lo = __ClassAtom(set);
            if (lo < 0)
                continue;

            if (__pos + 1 < __length && __Cur == '-' && __pattern[__pos + 1] != ']' && __pattern[__pos + 1] != '[')
            {
                __pos++;
                Int32 hi = __ClassAtom(set);
                if (hi < 0)
                    throw __Error("A class (\\d \\w \\s) can not be the end of a range");

                if (hi < lo)
                    throw __Error("[x-y] range in reverse order");

                set.AddRange(lo, hi);
            }
            else
            {
                set.AddChar(lo);
            }
        }

        return set;
    }

    // A character of a class: its value, or -1 when it is a class (\d \w \s, they are added to the set).
    private Int32 __ClassAtom(RegexCharClass set)
    {
        Char c = __Cur;
        __pos++;

        if (c != '\\')
            return (Int32)c;

        if (!__More)
            throw __Error("Illegal \\ at end of pattern");

        Char e = __Cur;
        __pos++;

        if (e == 'd') { __AddCategory(set, RegexCharClass.Digit); return -1; }
        if (e == 'D') { __AddCategory(set, RegexCharClass.NotDigit); return -1; }
        if (e == 'w') { __AddCategory(set, RegexCharClass.Word); return -1; }
        if (e == 'W') { __AddCategory(set, RegexCharClass.NotWord); return -1; }
        if (e == 's') { __AddCategory(set, RegexCharClass.Space); return -1; }
        if (e == 'S') { __AddCategory(set, RegexCharClass.NotSpace); return -1; }
        if (e == 'b') return 8;

        if (e == 'p' || e == 'P')
        {
            __AddProperty(set, e == 'P');
            return -1;
        }

        return __CharEscape(e);
    }

    ////////// ////////// ////////// ////////// //////////

    private Int32 __SaveFlags()
    {
        return (__ignoreCase ? 1 : 0) | (__multiline ? 2 : 0) | (__explicit ? 4 : 0) | (__singleline ? 8 : 0) | (__extended ? 16 : 0);
    }

    private Void __RestoreFlags(Int32 flags)
    {
        __ignoreCase = (flags & 1) != 0;
        __multiline = (flags & 2) != 0;
        __explicit = (flags & 4) != 0;
        __singleline = (flags & 8) != 0;
        __extended = (flags & 16) != 0;
    }

    // Reads the end of a group, ) .
    private Void __ExpectClose()
    {
        if (!__More || __Cur != ')')
            throw __Error("Not enough ')' characters");

        __pos++;
    }

    // A group: (...) (?:...) (?<name>...) (?'name'...) (?i-m:...) (?i-m) (?#comment), the assertions (?=...) (?!...)
    // (?<=...) (?<!...), the atomic group (?>...) and the condition (?(1)yes|no) (?(name)yes|no) (?(?=...)yes|no).
    private RegexNode? __ParseGroup()
    {
        Int32 start = __pos;
        __pos++;

        Int32 flags = __SaveFlags();

        String? name = null;
        String? balance = null;
        Int32 explicitNumber = -1;
        Boolean capture = !__explicit;

        if (__More && __Cur == '?')
        {
            __pos++;
            if (!__More)
                throw __Error("Unrecognized grouping construct");

            Char c = __Cur;
            if (c == '#')
            {
                while (__More && __Cur != ')')
                    __pos++;

                if (!__More)
                    throw __Error("Unterminated (?#...) comment");

                __pos++;
                return null;
            }

            capture = false;
            if (c == ':')
            {
                __pos++;
            }
            else if (c == '=' || c == '!')
            {
                __pos++;
                return __ParseAssertion(c == '!', false, flags);
            }
            else if (c == '>')
            {
                __pos++;
                RegexNode body = __ParseAlt();
                __ExpectClose();
                __RestoreFlags(flags);
                return RegexNode.Wrap(RegexNodeKind.Atomic, body);
            }
            else if (c == '(')
            {
                return __ParseCondition(flags);
            }
            else if (c == '<' && __pos + 1 < __length && (__pattern[__pos + 1] == '=' || __pattern[__pos + 1] == '!'))
            {
                Boolean negative = __pattern[__pos + 1] == '!';
                __pos += 2;
                return __ParseAssertion(negative, true, flags);
            }
            else if (c == '<' || c == '\'')
            {
                Char close = c == '<' ? '>' : '\'';
                __pos++;
                __ReadGroupNames(close);
                name = __nameA;
                balance = __nameB;
                explicitNumber = __nameANumber;
                capture = name != null || explicitNumber >= 0;
            }
            else
            {
                // The options: (?imnsx-imnsx) or (?imnsx-imnsx:...).
                Boolean on = true;
                while (true)
                {
                    if (!__More)
                        throw __Error("Unrecognized grouping construct");

                    Char o = __Cur;
                    __pos++;
                    if (o == '-')
                        on = false;
                    else if (o == 'i')
                        __ignoreCase = on;
                    else if (o == 'm')
                        __multiline = on;
                    else if (o == 'n')
                        __explicit = on;
                    else if (o == 's')
                        __singleline = on;
                    else if (o == 'x')
                        __extended = on;
                    else if (o == ')')
                        return null;
                    else if (o == ':')
                        break;
                    else
                        throw __Error("Unrecognized grouping construct");
                }
            }
        }

        RegexNode? group = null;
        if (capture || balance != null)
        {
            group = new RegexNode(RegexNodeKind.Group);
            if (capture)
            {
                group.GroupIndex = __groups.Count;
                __groups.Add(group);
                __names.Add(name);
                __explicitNumbers.Add(explicitNumber);
            }
            else
            {
                group.GroupNumber = -1;
            }

            if (balance != null)
            {
                group.Balance = true;
                group.Pos = start;
                if (Char.IsDigit(balance[0]))
                    group.RefNumber = __NumberOfName(balance);
                else
                    group.Name = balance;

                __refs.Add(group);
            }
        }

        RegexNode inner = __ParseAlt();
        __ExpectClose();
        __RestoreFlags(flags);

        if (group == null)
            return inner;

        group.Children = new List<RegexNode>();
        group.Children.Add(inner);
        return group;
    }

    // The part of an assertion, after (?= (?! (?<= (?<! : up to the ) .
    private RegexNode __ParseAssertion(Boolean negative, Boolean behind, Int32 flags)
    {
        RegexNode body = __ParseAlt();
        __ExpectClose();
        __RestoreFlags(flags);

        RegexNode look = RegexNode.Wrap(RegexNodeKind.Look, body);
        look.Negative = negative;
        look.Behind = behind;
        return look;
    }

    // The condition: at the ( after (? .
    private RegexNode __ParseCondition(Int32 flags)
    {
        Int32 at = __pos;
        RegexNode? test = null;
        String? name = null;
        Int32 number = -1;

        if (__pos + 1 < __length && __pattern[__pos + 1] == '?')
        {
            // (?(?=...)yes|no): the test is an assertion.
            RegexNode? assertion = __ParseGroup();
            if (assertion == null || assertion.Kind != RegexNodeKind.Look)
                throw new RegexParseException("The condition must be a group or an assertion", __pattern, at);

            test = assertion;
        }
        else
        {
            // (?(1)yes|no), (?(name)yes|no)
            __pos++;
            name = __ReadRefName(')');
            if (name.Length > 0 && Char.IsDigit(name[0]))
            {
                number = __NumberOfName(name);
                name = null;
            }
        }

        Int32 bodyStart = __pos;
        RegexNode yes = __ParseConcat();
        RegexNode no = new RegexNode(RegexNodeKind.Empty);
        if (__More && __Cur == '|')
        {
            __pos++;
            no = __ParseConcat();
            if (__More && __Cur == '|')
                throw __Error("Too many | in (?()|)");
        }

        __ExpectClose();
        __RestoreFlags(flags);

        if (test != null)
        {
            // test ? yes : no  is  (?:(?=test)yes|(?!test)no)
            RegexNode negated = RegexNode.Wrap(RegexNodeKind.Look, test.Children[0]);
            negated.Negative = !test.Negative;
            negated.Behind = test.Behind;

            List<RegexNode> first = new List<RegexNode>();
            first.Add(test);
            first.Add(yes);
            List<RegexNode> second = new List<RegexNode>();
            second.Add(negated);
            second.Add(no);

            List<RegexNode> both = new List<RegexNode>();
            both.Add(RegexNode.Of(RegexNodeKind.Concat, first));
            both.Add(RegexNode.Of(RegexNodeKind.Concat, second));
            return RegexNode.Of(RegexNodeKind.Alt, both);
        }

        RegexNode cond = new RegexNode(RegexNodeKind.Cond);
        cond.Children = new List<RegexNode>();
        cond.Children.Add(yes);
        cond.Children.Add(no);
        cond.Name = name;
        cond.RefNumber = number;
        cond.Pos = at;
        __refs.Add(cond);
        return cond;
    }

    // The number that a text of digits is.
    private Int32 __NumberOfName(String digits)
    {
        Int32 n = 0;
        for (Int32 i = 0; i < digits.Length; i++)
        {
            n = n * 10 + ((Int32)digits[i] - (Int32)'0');
            if (n > 100000)
                break;
        }

        return n;
    }

    // The name or the number of a group that is referred to, up to the closing character.
    private String __ReadRefName(Char close)
    {
        Int32 start = __pos;
        while (__More && __Cur != close)
        {
            Char c = __Cur;
            if (!(Char.IsLetterOrDigit(c) || c == '_'))
                throw __Error("Invalid group name");

            __pos++;
        }

        if (!__More || __pos == start)
            throw __Error("Invalid group name");

        String name = __pattern.Substring(start, __pos - start);
        __pos++;

        // A number is all digits.
        Boolean digits = Char.IsDigit(name[0]);
        for (Int32 i = 1; i < name.Length && digits; i++)
            digits = Char.IsDigit(name[i]);

        if (Char.IsDigit(name[0]) && !digits)
            throw __Error("Invalid group name");

        return name;
    }

    // The names of a group, up to the closing character: name, name1-name2 or -name2 (a balancing group), or the
    // number of the group instead of the first name. They are in __nameA (null when the group has none, or a number),
    // __nameANumber (-1 when it is not a number) and __nameB (null when it is not a balancing group).
    private Void __ReadGroupNames(Char close)
    {
        Int32 start = __pos;
        Int32 dash = -1;
        while (__More && __Cur != close)
        {
            Char c = __Cur;
            if (c == '-' && dash < 0)
                dash = __pos;
            else if (!(Char.IsLetterOrDigit(c) || c == '_'))
                throw __Error("Invalid group name");

            __pos++;
        }

        if (!__More)
            throw __Error("Invalid group name");

        String text = __pattern.Substring(start, __pos - start);
        __pos++;

        String first = dash < 0 ? text : text.Substring(0, dash - start);
        String? second = dash < 0 ? null : text.Substring(dash - start + 1);

        if (first.Length == 0 && second == null)
            throw __Error("Invalid group name");

        __nameANumber = -1;
        if (first.Length > 0)
        {
            if (__AllDigits(first))
            {
                Int32 number = __NumberOfName(first);
                if (number == 0 || number > 100000)
                    throw __Error("Capture group numbers must be greater than zero");

                __nameANumber = number;
                first = "";
            }
            else if (Char.IsDigit(first[0]))
            {
                throw __Error("Invalid group name");
            }
        }

        if (second != null)
        {
            // A number is all digits.
            Boolean digits = __AllDigits(second);
            if (second.Length == 0 || (Char.IsDigit(second[0]) && !digits))
                throw __Error("Invalid group name");
        }

        __nameA = first.Length == 0 ? null : first;
        __nameB = second;
    }

    private String __pattern;
    private Int32 __length;
    private Int32 __pos;
    private Int32 __min;
    private String? __nameA;
    private String? __nameB;
    private Int32 __max;

    private Boolean __ignoreCase;
    private Boolean __multiline;
    private Boolean __explicit;
    private Boolean __singleline;
    private Boolean __extended;

    private List<RegexNode> __groups;
    private List<String?> __names;
    private List<RegexNode> __refs;
    private List<Int32> __explicitNumbers;
    private List<Int32>? __defined;
    private Int32 __nameANumber;
    private Boolean __ecma;
    private Boolean __explicitAtStart;
}
