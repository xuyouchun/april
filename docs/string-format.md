# String.Format

`String.Format(String format, params Object[] args)` and `StringBuilder.AppendFormat(format, args)` make a text from a
format and arguments. The arguments are boxed as the arguments of any call that takes `Object`.

```csharp
String.Format("{0} + {1} = {2}", 1, 2, 3);        // 1 + 2 = 3
String.Format("[{0,6}] [{0,-6}]", 42);            // [    42] [42    ]
String.Format("{0:X4} {1:F2} {2:N0}", 255, 3.14159, 1234567);   // 00FF 3.14 1,234,567
String.Format("{{0}} is the index");              // {0} is the index
```

`Console.WriteLine(format, args)` and `Console.Write(format, args)` are `String.Format` written.

## The items of a format

`{index[,alignment][:format]}`

* `index`: the argument (0 is the first). An index that is not in the arguments is a `FormatException`.
* `alignment`: the width of the text of the argument; positive: the text is at the right, negative: at the left.
* `format`: given to the argument when it is `IFormattable` (the numbers); the other arguments have their text
  (`Object.ToString`) and ignore it. The text up to the `}` is the format (it has no `}`).
* `{{` and `}}` are the braces. A `{` that is not closed, a `}` alone, and an item without an index are a `FormatException`.
* A null argument is nothing.

## The formats of the numbers (`IFormattable`)

The numbers have `ToString(String? format)` and `ToString(String? format, IFormatProvider? provider)` (`n.ToString("X4")`). A format
is a standard one (a letter and the number of digits: `D5`, `F2`) or a custom one (`#,##0.00`). A format that is not valid is a
`FormatException`.

### Standard formats

| Letter | Integers | `Single`, `Double` |
|---|---|---|
| `D` | decimal, the precision is the least number of digits (`-00042`) | |
| `X` `x` | hexadecimal; a negative number is the bits of its type (`Int8` -1 is `FF`, `Int32` -1 is `FFFFFFFF`) | |
| `B` `b` | binary, the same bits | |
| `F` `f` | digits after the point, 2 by default (`42.00`) | the same, the exact value is rounded (a tie to the even digit: 2.5 is `2`, 0.125 is `0.12`) |
| `N` `n` | digits grouped by 3 with `,`, 2 digits after the point by default (`1,234,567.00`, `N0`) | the same |
| `P` `p` | the number times 100 with the percent symbol (`12.34%`), 2 digits by default | the same |
| `C` `c` | the currency symbol, then the number as `N` (`$1,234.50`, `-$1,234.50`), 2 digits by default | the same |
| `E` `e` | | `1.234568E+003`: 6 digits after the point by default, the exponent has a sign and 3 digits |
| `G` `g` | the text of the number, the precision is the least number of digits | the text of the number (`R` is the same) |

NaN and the infinities are the symbols of the info (`NaN`, `Infinity`, `-Infinity`) whatever the format is.

### Custom formats

A format that is not a letter and digits. The sections are split by `;`: the first is for all the numbers that have no section
of their own, the second for the negative numbers (no sign is added then), the third for zero.

| Character | |
|---|---|
| `0` | a digit, or 0 when the number has none |
| `#` | a digit, or nothing |
| `.` | the point of the decimals (it is written when a decimal is) |
| `,` | between two digit placeholders: the integer part is grouped by 3 |
| `%` | the number is multiplied by 100 and the percent symbol is written |
| `'text'` `"text"` `\c` | the characters as they are |
| the others | as they are |

```csharp
1234.5678.ToString("#,##0.00")        // 1,234.57
0.256.ToString("0.0%")                // 25.6%
5551234567.ToString("(000) 000-0000") // (555) 123-4567
5.ToString("0;(0);zero")              // 5      (-5: (5)      0: zero)
```

The scaling comma (`0,`) and the exponent (`0.0E+00`) are not supported (`FormatException`).

### Symbols: `NumberFormatInfo`

`System.Globalization.NumberFormatInfo` has the symbols: `NegativeSign`, `NumberDecimalSeparator`, `NumberGroupSeparator`,
`PercentSymbol`, `CurrencySymbol`, `NaNSymbol`, `PositiveInfinitySymbol`, `NegativeInfinitySymbol`, and the digits that F/N
(`NumberDecimalDigits`), P and C have by default. There are no cultures: `NumberFormatInfo.CurrentInfo` has the symbols of the
United States (`.`, `,`, `$`, `%`), `InvariantInfo` is the same with the currency symbol `¤`. Both are read only: `Clone()`
makes one that is changed. An info is the `IFormatProvider` of the numbers and of `String.Format(provider, format, args)` and
`StringBuilder.AppendFormat(provider, format, args)`; without a provider the symbols are those of `CurrentInfo`.

```csharp
NumberFormatInfo european = NumberFormatInfo.CurrentInfo.Clone();
european.NumberDecimalSeparator = ",";
european.NumberGroupSeparator = ".";
1234567.891.ToString("N2", european)  // 1.234.567,89
```

## Limits

* No cultures, no `ICustomFormatter`; a `IFormatProvider` of the program gives its info by `GetFormat(typeof(NumberFormatInfo))`.
* `D` and `X` are not formats of the floating point numbers (`FormatException`); the number of digits of a format is at most 99.
* A tie in a custom format or in `F` is rounded to the even digit (the exact value of the number is rounded).
