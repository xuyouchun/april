// Regex: the patterns that are not valid are a RegexParseException (an ArgumentException) with the pattern and the
// offset; the options and the syntax that are not supported are told in the message.

/* EXPECT

( | offset 1 | True
(a | offset 2 | True
) | offset 0 | True
a) | offset 1 | True
(a)) | offset 3 | True
[a | offset 2 | True
[ | offset 1 | True
[] | offset 2 | True
[a- | offset 3 | True
*a | offset 0 | True
+ | offset 0 | True
a|* | offset 2 | True
(?:*) | offset 3 | True
a** | offset 2 | True
a+* | offset 2 | True
a{2}{3} | offset 4 | True
a{2,1} | offset 6 | True
a{99999999} | offset 7 | True
\ | offset 1 | True
a\ | offset 2 | True
\q | offset 2 | True
\xZ | offset 2 | True
\x4 | offset 3 | True
\u12 | offset 4 | True
\c | offset 2 | True
[z-a] | offset 4 | True
[\d-z]x | valid
[a-\d] | offset 5 | True
[a-z-[b]c] | offset 8 | True
(?<n | offset 4 | True
(?<>a) | offset 4 | True
(?<1a>x) | offset 6 | True
(?z) | offset 3 | True
(? | offset 2 | True
(?#abc | offset 6 | True
\1 | offset 0 | True
(a)\2 | offset 3 | True
(a)\k<n> | offset 3 | True
\k | offset 2 | True
\k<> | offset 3 | True
\k<a-b> | offset 4 | True
(?<=a+)b | offset 4 | True
(?<!a|bc)d | offset 4 | True
(?<=(a)\1)b | offset 4 | True
(?=a | offset 4 | True
(?>a | offset 4 | True
(?(1)a|b|c) | offset 8 | True
(?(2)a|b)(a) | offset 2 | True
(?(n)a|b) | offset 2 | True
(?(a | offset 4 | True
(?(:a)b) | offset 3 | True
\p{L} | offset 2 | True
\P{L} | offset 2 | True
[\p{L}] | offset 3 | True
\G | offset 2 | True
(?<a-b>x) | offset 4 | True
{ | valid
a{ | valid
} | valid
] | valid
a{1,x} | valid
[a-] | valid
[-a] | valid
[a\]] | valid
[^]a] | valid
() | valid
(|) | valid
a||b | valid
(?i) | valid
(?i-s:a) | valid
(?<name>a)(?'other'b) | valid
\/\-\@\" | valid
\b\B\A\z\Z\n\r\t\f\v\e\a\0\012\cA\x41\u0041 | valid
a{1}{ | valid
(a)\1 | valid
(a)\1\1 | valid
(?<n>a)\k<n>\k'n' | valid
(?<n>a)\k<1> | valid
(a)(?(1)b) | valid
(a)?(?(1)b|c) | valid
(?<n>a)?(?(n)b|c) | valid
(?(?=a)ab|cd) | valid
(?(?<!a)b|c) | valid
(?=a)(?!b)(?<=c)(?<!d)(?>e) | valid
(?<=ab|cd)x | valid
(?<=a{2}[bc]\d)x | valid
(?<=(a)(b))x | valid
a\10 | valid
(a)(b)(c)(d)(e)(f)(g)(h)(i)(j)\10 | valid
\101\0 | valid
message True True True
unsupported True
look behind True 4
undefined True 3
righttoleft
ecmascript
ignored True True
null
null input
done

*/

import System;

using System;
using System.Text.RegularExpressions;

class Program
{
    static void Bad(String pattern)
    {
        try
        {
            new Regex(pattern);
            Console.WriteLine(pattern + " | valid");
        }
        catch (RegexParseException e)
        {
            Console.WriteLine(pattern + " | offset " + e.Offset + " | " + e.Pattern.Equals(pattern));
        }
    }

    static void Ok(String pattern)
    {
        try
        {
            new Regex(pattern);
            Console.WriteLine(pattern + " | valid");
        }
        catch (RegexParseException e)
        {
            Console.WriteLine(pattern + " | error offset " + e.Offset);
        }
    }

    [EntryPoint]
    public static void Main()
    {
        // The parentheses and the brackets.
        Bad("(");
        Bad("(a");
        Bad(")");
        Bad("a)");
        Bad("(a))");
        Bad("[a");
        Bad("[");
        Bad("[]");
        Bad("[a-");

        // The quantifiers.
        Bad("*a");
        Bad("+");
        Bad("a|*");
        Bad("(?:*)");
        Bad("a**");
        Bad("a+*");
        Bad("a{2}{3}");
        Bad("a{2,1}");
        Bad("a{99999999}");

        // The escapes and the classes.
        Bad("\\");
        Bad("a\\");
        Bad("\\q");
        Bad("\\xZ");
        Bad("\\x4");
        Bad("\\u12");
        Bad("\\c");
        Bad("[z-a]");
        Bad("[\\d-z]x");
        Bad("[a-\\d]");
        Bad("[a-z-[b]c]");

        // The names of the groups and the constructs.
        Bad("(?<n");
        Bad("(?<>a)");
        Bad("(?<1a>x)");
        Bad("(?z)");
        Bad("(?");
        Bad("(?#abc");

        // The references to a group, the assertions, the atomic groups and the conditions.
        Bad("\\1");
        Bad("(a)\\2");
        Bad("(a)\\k<n>");
        Bad("\\k");
        Bad("\\k<>");
        Bad("\\k<a-b>");
        Bad("(?<=a+)b");
        Bad("(?<!a|bc)d");
        Bad("(?<=(a)\\1)b");
        Bad("(?=a");
        Bad("(?>a");
        Bad("(?(1)a|b|c)");
        Bad("(?(2)a|b)(a)");
        Bad("(?(n)a|b)");
        Bad("(?(a");
        Bad("(?(:a)b)");

        // The syntax that is not supported yet.
        Bad("\\p{L}");
        Bad("\\P{L}");
        Bad("[\\p{L}]");
        Bad("\\G");
        Bad("(?<a-b>x)");

        // The patterns that look wrong and are valid.
        Ok("{");
        Ok("a{");
        Ok("}");
        Ok("]");
        Ok("a{1,x}");
        Ok("[a-]");
        Ok("[-a]");
        Ok("[a\\]]");
        Ok("[^]a]");
        Ok("()");
        Ok("(|)");
        Ok("a||b");
        Ok("(?i)");
        Ok("(?i-s:a)");
        Ok("(?<name>a)(?'other'b)");
        Ok("\\/\\-\\@\\\"");
        Ok("\\b\\B\\A\\z\\Z\\n\\r\\t\\f\\v\\e\\a\\0\\012\\cA\\x41\\u0041");
        Ok("a{1}{");
        Ok("(a)\\1");
        Ok("(a)\\1\\1");
        Ok("(?<n>a)\\k<n>\\k'n'");
        Ok("(?<n>a)\\k<1>");
        Ok("(a)(?(1)b)");
        Ok("(a)?(?(1)b|c)");
        Ok("(?<n>a)?(?(n)b|c)");
        Ok("(?(?=a)ab|cd)");
        Ok("(?(?<!a)b|c)");
        Ok("(?=a)(?!b)(?<=c)(?<!d)(?>e)");
        Ok("(?<=ab|cd)x");
        Ok("(?<=a{2}[bc]\\d)x");
        Ok("(?<=(a)(b))x");
        Ok("a\\10");
        Ok("(a)(b)(c)(d)(e)(f)(g)(h)(i)(j)\\10");
        Ok("\\101\\0");

        // The message tells the pattern and the offset.
        try
        {
            new Regex("ab(c");
        }
        catch (ArgumentException e)
        {
            Console.WriteLine("message " + e.Message.Contains("ab(c") + " " + e.Message.Contains("offset 4") + " " + (e is RegexParseException));
        }

        try
        {
            new Regex("\\p{L}");
        }
        catch (RegexParseException e)
        {
            Console.WriteLine("unsupported " + e.Message.Contains("not supported"));
        }

        try
        {
            new Regex("(?<=a+)b");
        }
        catch (RegexParseException e)
        {
            Console.WriteLine("look behind " + e.Message.Contains("variable length") + " " + e.Offset);
        }

        try
        {
            new Regex("(a)\\3");
        }
        catch (RegexParseException e)
        {
            Console.WriteLine("undefined " + e.Message.Contains("undefined group number 3") + " " + e.Offset);
        }

        // The options that are not supported.
        try
        {
            new Regex("a", RegexOptions.RightToLeft);
        }
        catch (NotSupportedException)
        {
            Console.WriteLine("righttoleft");
        }

        try
        {
            new Regex("a", RegexOptions.ECMAScript);
        }
        catch (NotSupportedException)
        {
            Console.WriteLine("ecmascript");
        }

        // The options that have no effect.
        Console.WriteLine("ignored " + new Regex("a", RegexOptions.Compiled).IsMatch("a") + " " + new Regex("a", RegexOptions.CultureInvariant).IsMatch("a"));

        try
        {
            new Regex(null);
        }
        catch (ArgumentException)
        {
            Console.WriteLine("null");
        }

        try
        {
            new Regex("a").IsMatch(null);
        }
        catch (ArgumentException)
        {
            Console.WriteLine("null input");
        }

        Console.WriteLine("done");
    }
}
