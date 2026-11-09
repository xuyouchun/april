# System.Text

The classes of `core_lib/System/Text`. The members of `String` and `Char` are in `core_lib/System/String.cs` and
`Char.cs` (see `characters.md` for the characters and the layout of a string).

## StringBuilder

A string that can be changed. The characters are in a `Char[]` that doubles when it is full, so appending does
not depend on the length of the text. `Append<T>(T) where T : IStringable` takes the numbers, `Boolean` and the
other types that have a text, without boxing them; `Append(Object?)` takes the others.

The others: `Append(Char[], start, count)`, `Append(StringBuilder)` (also itself), `AppendJoin(separator, String[])`,
`Insert(index, String, repeat)` and `Insert(index, Char[])`, `Replace(old, new, start, count)` for strings and
characters, `CopyTo(start, Char[], index, count)`, and `Equals(StringBuilder)` (the same characters). The
indexer, `Length` (a larger value fills with the character 0) and `Capacity` were already there.

## Rune

`Rune` is a Unicode scalar value (U+0000 to U+10FFFF, not the surrogates): a whole character of any plane, where
a `Char` is a unit of UTF-16. A struct that holds an `Int32`.

* `new Rune(Int32)`, `new Rune(Char)`, `new Rune(Char high, Char low)`; a value that is not a scalar value
  (a surrogate, over U+10FFFF) is an `ArgumentException`. `(Rune)n`, `(Rune)c` and `(Int32)r` are explicit.
* `Value`, `IsBmp`, `Plane`, `Utf16SequenceLength` (1 or 2), `Utf8SequenceLength` (1 to 4), `Rune.ReplacementChar`,
  `Rune.IsValid(Int32)`.
* On a string: `Rune.GetRuneAt(s, index)` (a pair is one rune; a surrogate that has no pair is U+FFFD),
  `Rune.CountRunes(s)`, `Rune.ToRunes(s)`. `StringBuilder.Append(Rune)`.
* `IsLetter`, `IsDigit`, `IsLetterOrDigit`, `IsWhiteSpace`, `IsUpper`, `IsLower`, `ToUpper`, `ToLower` are those of
  `Char`: a rune that is not in the basic plane has none of the properties and its case is not changed.
* `==`, `!=`, `<`, `>`, `<=`, `>=`, `CompareTo`, `Equals`, `GetHashCode`, `ToString` (one or two `Char`s).

There is no `TryCreate` (the library has no `out` parameter yet) and no `EnumerateRunes`.

## Encoding

`Encoding` is the conversion between a `String` and bytes (`UInt8[]`). A character that cannot be coded is
replaced, so the conversions do not fail because of the text; a range that is not in an array is an
`OutOfRangeException`.

| Encoding | `Name` | Bytes | A character that cannot be coded |
|---|---|---|---|
| `Encoding.UTF8` | `utf-8` | 1 to 4 | U+FFFD (a surrogate that has no pair) |
| `Encoding.Unicode` | `utf-16` | 2 per unit, least significant first | the units are kept |
| `Encoding.BigEndianUnicode` | `utf-16BE` | 2 per unit, most significant first | the units are kept |
| `Encoding.UTF32` | `utf-32` | 4, least significant first | U+FFFD |
| `Encoding.ASCII` | `us-ascii` | 1 | `?` (a pair of surrogates is one `?`) |
| `Encoding.Latin1` | `iso-8859-1` | 1 | `?` |
| `Encoding.GB18030` | `gb18030` | 1, 2 or 4 | U+FFFD (a surrogate that has no pair) |

Decoding: the bytes that are not valid are U+FFFD. For UTF-8 these are a lead byte that is not one (`80` to `C1`,
`F5` to `FF`), a sequence that is too long (`C0 80`), a surrogate (`ED A0 80`), a value over U+10FFFF, and a
sequence that is not complete (the good bytes of it are one error). UTF-16 and UTF-32 take a byte or bytes that
are left at the end as one U+FFFD. No encoding writes or reads a byte order mark (see `GetPreamble`).

The members: `GetByteCount`, `GetMaxByteCount`, `GetBytes(String)` / `GetBytes(Char[])` /
`GetBytes(String, UInt8[], Int32)`, `GetString(UInt8[])` / `GetString(UInt8[], Int32, Int32)`, `GetChars`,
`GetCharCount`. A new encoding derives from `Encoding` and gives `WriteBytes` and `ReadString` (the ranges are
checked by the base class), `GetByteCount` and `GetMaxByteCount`.

### Finding an encoding

`Encoding.GetEncoding(String name)` and `GetEncoding(Int32 codePage)` give the shared encoding (the same object as
`Encoding.UTF8` and the others); the case and the spaces around the name do not matter; an `ArgumentException` when
the encoding is not known. `GetEncoding(name | codePage, EncoderFallback, DecoderFallback)` gives a new encoding with the
fallbacks. `Encoding.Default` is UTF-8. `Encoding.Convert(source, destination, bytes[, index, count])` is the bytes of
the text of the bytes in another encoding.

| Encoding | Code page | `EncodingName` | Names |
|---|---|---|---|
| `Encoding.UTF8` | 65001 | Unicode (UTF-8) | `utf-8`, `utf8`, `unicode-1-1-utf-8`, `utf_8` |
| `Encoding.Unicode` | 1200 | Unicode | `utf-16`, `utf16`, `utf-16le`, `unicode`, `ucs-2`, `ucs-2le` |
| `Encoding.BigEndianUnicode` | 1201 | Unicode (Big-Endian) | `utf-16be`, `unicodefffe`, `ucs-2be` |
| `Encoding.UTF32` | 12000 | Unicode (UTF-32) | `utf-32`, `utf32`, `utf-32le` |
| `Encoding.ASCII` | 20127 | US-ASCII | `us-ascii`, `ascii`, `ansi_x3.4-1968`, `iso646-us`, `us` |
| `Encoding.Latin1` | 28591 | Western European (ISO) | `iso-8859-1`, `latin1`, `latin-1`, `iso8859-1`, `l1`, `cp819`, `ibm819` |
| `Encoding.GB18030` | 54936 | Chinese Simplified (GB18030) | `gb18030`, `gb-18030`, `x-gb18030`, `cp54936` |

The properties of an encoding: `Name` and `WebName` (the same), `CodePage`, `EncodingName`, `IsSingleByte`. A new
encoding gives them (`CodePage`, `EncodingName`, `IsSingleByte` can be overridden); it is not found by `GetEncoding`.
UTF-32 of the most significant byte first and the code pages of Windows (437, 936, 1252 ...) are not there; `gbk` and `gb2312` are not names of `GB18030` (the sets of characters are smaller).

### EncoderFallback

A character that an encoding cannot code is replaced (U+FFFD, or `?` in ASCII and Latin1) until a fallback is given:
`encoding.WithEncoderFallback(fallback)` makes an encoding like it (the first one is not changed) that handles those
characters by the fallback. What cannot be coded: a surrogate that has no pair (UTF-8, UTF-32), a character over the
limit and a pair of surrogates (ASCII, Latin1); UTF-16 codes every unit. A pair of surrogates is one unknown text of
two characters.

* `EncoderFallback.ReplacementFallback` (`EncoderReplacementFallback`, `?`; `new EncoderReplacementFallback("[?]")` any
  string that has no surrogate without a pair) and `EncoderFallback.ExceptionFallback` (`EncoderExceptionFallback`).
* A new fallback derives from `EncoderFallback` and gives `GetReplacement(unknown, index)`: the text that is coded in
  its place (the replacement is not examined again).
* `EncoderFallbackException` (an `ArgumentException`): `UnknownText` and `Index` (in the text that is coded).
* `encoding.EncoderFallback` is the fallback of the encoding; without one it is an `EncoderReplacementFallback` of the
  replacement of the encoding.
* The fallback is used by `GetBytes`, `GetByteCount` and an `Encoder` of the encoding (a high surrogate that is kept for
  the next part and is left at the end, with `flush`, is unknown).
* A new encoding gives `CountBytes`, `WriteBytes`, `ReadString`, `GetMaxByteCount`, and `CanEncode` (default: not a
  surrogate that has no pair), `DefaultReplacement` and `MakeCopy` (an encoding that has no fallback).

### DecoderFallback

The same for the bytes that are not valid: `encoding.WithDecoderFallback(fallback)` makes an encoding like it (the
encoder fallback is kept) that gives the bytes of an error to the fallback. `DecoderFallback.ReplacementFallback`
(`DecoderReplacementFallback`, U+FFFD; any string), `DecoderFallback.ExceptionFallback` (`DecoderExceptionFallback`:
`DecoderFallbackException` with `BytesUnknown` and `Index` in the array that is decoded), or a new fallback that gives
`GetReplacement(bytesUnknown, index)`. `encoding.DecoderFallback` is the fallback of the encoding. `Decoder`
objects use it too: the bytes of a character that is cut wait for the next part, and are an error when they are left
at the end (`flush`).

The errors are the ones that the encoding had (a replacement fallback with U+FFFD gives the text of an encoding that
has none): UTF-8, a lead byte that is not one (`80` to `C1`, `F5` to `FF`) is one error, the good bytes of a sequence that
is not complete are one error, a sequence that is too long, a surrogate or a value over U+10FFFF is an error of its lead
byte and each byte that follows is an error of its own; UTF-16, the byte that is left at the end (the units are all
valid); UTF-32, a value that is not a scalar value (4 bytes) and the bytes that are left at the end (one error); ASCII,
a byte over 127 (Latin1 has none). A new encoding gives `CheckBytes(bytes, index, end)` (the size of the valid unit, or
minus the size of the error) to have it.

### GB18030

`Encoding.GB18030` (`new GB18030Encoding()`, code page 54936, `EncodingName` Chinese Simplified (GB18030)) is the encoding of
the Chinese characters, GB 18030 (the mapping of the 2005 edition, the one of the `gb18030` codec of Python). The
characters up to U+007F are a byte; the others are two bytes (lead `81` to `FE`, trail `40` to `7E` or `80` to `FE`) or four
bytes (`81`-`FE`, `30`-`39`, `81`-`FE`, `30`-`39`; the characters over U+FFFF are the four bytes from `90 30 81 30`).
Every character of Unicode has a code, so nothing is lost but a surrogate that has no pair (U+FFFD).

The table is the resource `System.Text.gb18030.bin` of the assembly System (50 KB: 23940 characters of the codes of two
bytes, and 206 ranges of the codes of four bytes of the basic plane), made by `tools/gen_gb18030.py` and read when the
encoding is first used (see `resources.md`); the table of the characters to the codes is made from it when a text is first
coded.

Decoding: a byte `80` or `FF`, a lead byte that is not followed by a trail byte or by a digit, and a code of four bytes
whose third or fourth byte is not right are an error of one byte (the lead byte); a code of four bytes that is complete and is
not a character (the numbers between the ranges, over U+10FFFF) is an error of four bytes; the bytes at the end that are the
beginning of a code are one error. A `Decoder` keeps them for the next part.

### In parts: Encoder, Decoder, preamble

`Encoding.GetEncoder()` and `GetDecoder()` convert text that comes in parts (the blocks of a file or a stream). The
result of all the parts is the result of the whole: a high surrogate at the end of a part (encoder), or the first
bytes of a UTF-8 sequence or half of a UTF-16 unit or UTF-32 character (decoder), wait for the next part. The last
part has `flush = true`: what is left is a character that cannot be coded (U+FFFD or the replacement of the
encoding). `Encoder.GetBytes(String | Char[], ..., flush)`, `GetByteCount(String, flush)` (the state is not
changed), `Decoder.GetString(UInt8[], [index, count,] flush)`, `GetChars`, `PendingByteCount`, `Reset`. A new
encoding that has characters of more than one byte gives `GetCompleteLength(bytes, count)` (the number of bytes that
end at a whole character).

`GetPreamble()` is the byte order mark (`UInt8[]`, empty by default): `new UTF8Encoding(true)` gives `EF BB BF`,
`new UnicodeEncoding(bigEndian, true)` `FF FE` or `FE FF`, `new UTF32Encoding(true)` `FF FE 00 00`. `GetBytes`
does not write it and `GetString` does not skip it; the program that writes a file writes the preamble first.

UTF-8 is made by internal functions (`Utf8_GetByteCount`, `Utf8_GetBytes`, `Utf8_GetString` in `rtlib/string.cpp`);
the others are written in the library.

## Limits of the library

* The static fields of a library have no initial values: the static constructors are called at the start of the
  entry point of the program, and only for the types of its assembly. A library makes what it keeps in a static
  field when it is first asked for (see the encodings of `Encoding`).
