/*
 *  System.dll
 *
 *  RegexUnicode: the names of the categories of Unicode and of the blocks, for \p{..} and \P{..}.
 */

namespace Text.RegularExpressions;

////////// ////////// ////////// ////////// //////////

internal static class RegexUnicode
{
    // The set of the categories (a bit for each number of System.Globalization.UnicodeCategory) that has the name: the
    // categories of two letters (Lu, Nd ...) and the groups of one letter (L, M, N, Z, C, P, S). 0 when there is no such
    // name.
    internal static Int32 CategoryMask(String name)
    {
        if (name.Length == 1)
        {
            switch (name[0])
            {
                case 'L':
                {
                    return 0x1F;            // Lu Ll Lt Lm Lo
                }

                case 'M':
                {
                    return 0xE0;            // Mn Mc Me
                }

                case 'N':
                {
                    return 0x700;           // Nd Nl No
                }

                case 'Z':
                {
                    return 0x3800;          // Zs Zl Zp
                }

                case 'C':
                {
                    return 0x2003C000;      // Cc Cf Cs Co Cn
                }

                case 'P':
                {
                    return 0x1FC0000;       // Pc Pd Ps Pe Pi Pf Po
                }

                case 'S':
                {
                    return 0x1E000000;      // Sm Sc Sk So
                }

                default:
                {
                    return 0;
                }
            }
        }

        if (name.Length == 2)
        {
            String[] names = new String[] {
                "Lu", "Ll", "Lt", "Lm", "Lo", "Mn", "Mc", "Me", "Nd", "Nl", "No", "Zs", "Zl", "Zp", "Cc", "Cf",
                "Cs", "Co", "Pc", "Pd", "Ps", "Pe", "Pi", "Pf", "Po", "Sm", "Sc", "Sk", "So", "Cn"
            };

            for (Int32 i = 0; i < names.Length; i++)
            {
                if (names[i] == name)
                    return 1 << i;
            }
        }

        return 0;
    }

    // The set of the categories of the characters of a word (\w): the letters, the marks that do not take room (Mn), the
    // decimal digits (Nd) and the connector punctuation (Pc).
    internal const Int32 WordMask = 0x4013F;

    // The number of the block that has the name (IsGreek, IsBasicLatin ...), -1 when there is none. The blocks of the
    // basic plane that are used most are here, with the names of .NET.
    internal static Int32 FindBlock(String name)
    {
        String[] names = __Names();
        for (Int32 i = 0; i < names.Length; i++)
        {
            if (names[i] == name)
                return i;
        }

        return -1;
    }

    // The first character of the block.
    internal static Int32 BlockStart(Int32 block) => __Ranges()[block * 2];

    // The last character of the block.
    internal static Int32 BlockEnd(Int32 block) => __Ranges()[block * 2 + 1];

    private static String[] __Names()
    {
        return new String[] {
            "IsBasicLatin", "IsLatin-1Supplement", "IsLatinExtended-A", "IsLatinExtended-B", "IsIPAExtensions",
            "IsSpacingModifierLetters", "IsCombiningDiacriticalMarks", "IsGreek", "IsGreekandCoptic", "IsCyrillic",
            "IsCyrillicSupplement", "IsArmenian", "IsHebrew", "IsArabic", "IsSyriac", "IsThaana", "IsDevanagari",
            "IsBengali", "IsGurmukhi", "IsGujarati", "IsThai", "IsLao", "IsTibetan", "IsGeorgian", "IsHangulJamo",
            "IsEthiopic", "IsCherokee", "IsKhmer", "IsMongolian", "IsLatinExtendedAdditional", "IsGreekExtended",
            "IsGeneralPunctuation", "IsSuperscriptsandSubscripts", "IsCurrencySymbols", "IsLetterlikeSymbols",
            "IsNumberForms", "IsArrows", "IsMathematicalOperators", "IsMiscellaneousTechnical", "IsBoxDrawing",
            "IsBlockElements", "IsGeometricShapes", "IsMiscellaneousSymbols", "IsDingbats", "IsCJKRadicalsSupplement",
            "IsCJKSymbolsandPunctuation", "IsHiragana", "IsKatakana", "IsBopomofo", "IsHangulCompatibilityJamo",
            "IsEnclosedCJKLettersandMonths", "IsCJKCompatibility", "IsCJKUnifiedIdeographsExtensionA",
            "IsCJKUnifiedIdeographs", "IsHangulSyllables", "IsHighSurrogates", "IsHighPrivateUseSurrogates",
            "IsLowSurrogates", "IsPrivateUse", "IsCJKCompatibilityIdeographs", "IsAlphabeticPresentationForms",
            "IsArabicPresentationForms-A", "IsHalfwidthandFullwidthForms", "IsSpecials"
        };
    }

    // The ranges of the blocks: the first and the last character, in the order of the names.
    private static Int32[] __Ranges()
    {
        return new Int32[] {
            0x0000, 0x007F, 0x0080, 0x00FF, 0x0100, 0x017F, 0x0180, 0x024F, 0x0250, 0x02AF,
            0x02B0, 0x02FF, 0x0300, 0x036F, 0x0370, 0x03FF, 0x0370, 0x03FF, 0x0400, 0x04FF,
            0x0500, 0x052F, 0x0530, 0x058F, 0x0590, 0x05FF, 0x0600, 0x06FF, 0x0700, 0x074F, 0x0780, 0x07BF, 0x0900, 0x097F,
            0x0980, 0x09FF, 0x0A00, 0x0A7F, 0x0A80, 0x0AFF, 0x0E00, 0x0E7F, 0x0E80, 0x0EFF, 0x0F00, 0x0FFF, 0x10A0, 0x10FF, 0x1100, 0x11FF,
            0x1200, 0x137F, 0x13A0, 0x13FF, 0x1780, 0x17FF, 0x1800, 0x18AF, 0x1E00, 0x1EFF, 0x1F00, 0x1FFF,
            0x2000, 0x206F, 0x2070, 0x209F, 0x20A0, 0x20CF, 0x2100, 0x214F,
            0x2150, 0x218F, 0x2190, 0x21FF, 0x2200, 0x22FF, 0x2300, 0x23FF, 0x2500, 0x257F,
            0x2580, 0x259F, 0x25A0, 0x25FF, 0x2600, 0x26FF, 0x2700, 0x27BF, 0x2E80, 0x2EFF,
            0x3000, 0x303F, 0x3040, 0x309F, 0x30A0, 0x30FF, 0x3100, 0x312F, 0x3130, 0x318F,
            0x3200, 0x32FF, 0x3300, 0x33FF, 0x3400, 0x4DBF,
            0x4E00, 0x9FFF, 0xAC00, 0xD7AF, 0xD800, 0xDB7F, 0xDB80, 0xDBFF,
            0xDC00, 0xDFFF, 0xE000, 0xF8FF, 0xF900, 0xFAFF, 0xFB00, 0xFB4F,
            0xFB50, 0xFDFF, 0xFF00, 0xFFEF, 0xFFF0, 0xFFFF
        };
    }
}
