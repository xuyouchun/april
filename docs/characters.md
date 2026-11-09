# Characters and strings

`Char` is 16 bits, and the characters of a `String` are 16 bits (UTF-16), on every platform. A
character that is not in the basic plane (U+10000 and above, e.g. an emoji) is two `Char`s of a string,
a pair of surrogates: `"😀".Length` is 2 everywhere.

## Two kinds of characters in the C++ code

| Type | Used by | Width |
|---|---|---|
| `char_t` (`wchar_t`) | the host: the compiler, the command line, the messages, `std::wcout` | 16 bits on Windows, 32 bits on Linux |
| `rt_char_t` (`char16_t`) | the virtual machine: the values of `Char`, the data of `String`, the names that the reflection gives | 16 bits |

`ustring_t` is `std::basic_string<rt_char_t>`. The numeric type of the values `char_` (`tvalue_t`,
`cvalue_t`, the vtype traits) is `rt_char_t`; the macro `__X_NUMERIC(type)` is the C++ type of a numeric
type, use it when a macro is made for each of the types.

The conversions are in `common/__string.h`:

* `to_ustring(const char_t *)`, `to_host_string(const rt_char_t *)`: a character of the host that is
  wide becomes a pair of surrogates, and back.
* `utf8_to_ustring`, `ustring_to_utf8`: UTF-8 to and from UTF-16, with the replacement character
  (U+FFFD) for what is not valid.
* `utf8_locale()`: the locale of the wide streams that write UTF-8. `std::codecvt_utf8<wchar_t>` is
  UCS-2 when `wchar_t` has 16 bits: a pair of surrogates stops the stream. This locale keeps the high
  surrogate in the state (the stdio streams convert one character at a time).

## Where the boundaries are

* **Source code**: `lib::read_all_text` reads UTF-8 (a byte order mark is skipped), `write_all_text`
  writes UTF-8. A character literal is a character of 16 bits: a character that does not fit is an
  error (`'😀'`: it is a string of two Chars). `\uXXXX` is a character of 16 bits, `\UXXXXXXXX` any
  character (a pair of surrogates when it is not in the basic plane, in a string).
* **Assemblies (`.apl`)**: the strings are UTF-8, a `Char` constant is 16 bits, whatever the platform is.
  The runtime pool of the strings (`rt_spool_t`) keeps them as UTF-16 (`rt_string_t::s`); the names that
  the reflection gives (`rt_reflect_member_t::name`) are UTF-16 too.
* **Runtime library (`rtlib`)**: the functions of the strings work on `const rt_char_t *`; `Console_*`
  convert to the host (`to_host_string`) when they write.
* **Native programs**: `Char` is `i16`, the strings are made with `add_wstring(units, 16)`, the output
  uses `utf8_locale()`.
* **The constants that a runtime assembly gives** (`cvalue_t`): a string is made by
  `cvalue_t(const rt_char_t *)` and read by `rt_string()`. Do not read `string` of such a value, it has
  the characters of the virtual machine, not those of the host.
* **Size in memory**: a `Char` is 2 bytes in arrays, fields and structs, in the interpreter, the JIT and the
  native programs (`get_vtype_size(vtype_t::char_)` and `size_of(xil_type_t::char_)` are 2). The functions
  of `rtlib` that read or write the elements of a `Char[]` (`String_FromChars`, `String_CopyTo`) rely on it.
* **Layout of a string**: the value of a `String` is a pointer to its first character. The 8 bytes before
  it are the head: the length (`int32`) and 4 bytes that are reserved. The characters end with 0 for the
  C++ functions, but the length is what counts, so `Length`, `GetChar` and the like are O(1) and a string
  made by the program (concatenation, `StringBuilder`, `String.FromChars`) can have the character 0. The
  helpers are `rt_string_length`, `rt_string_bytes`, `rt_string_init` and `rt_empty_string` (`rt.h`). Every
  place that makes a string must write the head: `rtlib` (`__new_string`), the pool of strings of the
  runtime (`rt_spool_t::to_sid`), and the constants of the native programs (`add_wstring(units, 16)`). A
  null pointer is the empty string. The memory of a native string is a block of the collector that the
  pointer points into (the collector takes the pointers into a block).
* **The character 0 in a literal**: `"a\u0000b"` keeps its three characters. The compiler carries the length of a string constant (`token_data_t::string_length`, `cvalue_t::string_length`, `get_string()`), the constants of the assembly are in UTF-8 and the character 0 is written as `C0 80` ("modified UTF-8", the strings of the heap end with a byte 0), and `utf8_to_ustring` reads it back.
