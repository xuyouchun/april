/*
 *  System.dll
 *
 *  RegexOptions: the options of a regular expression.
 */

namespace Text.RegularExpressions;

////////// ////////// ////////// ////////// //////////

// The options of a regular expression. They are combined with |. The values are those of .NET.
// Compiled and CultureInvariant are accepted and have no effect; RightToLeft and ECMAScript are not
// supported (the constructor of Regex throws a NotSupportedException).
public enum RegexOptions : Int32
{
    None = 0,
    IgnoreCase = 1,
    Multiline = 2,
    ExplicitCapture = 4,
    Compiled = 8,
    Singleline = 16,
    IgnorePatternWhitespace = 32,
    RightToLeft = 64,
    ECMAScript = 256,
    CultureInvariant = 512;
}
