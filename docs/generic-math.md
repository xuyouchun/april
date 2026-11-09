# Generic math

The integer types implement the interfaces of the numbers, so a generic method can use their operators, constants and
methods through a constraint:

```csharp
using System.Numerics;

static T Sum<T>(T[] items) where T : INumberBase<T>
{
    T total = T.Zero;
    for (Int32 k = 0; k < items.Length; k++)
        total = total + items[k];

    return total;
}
```

The interfaces are all in `core_lib/System/Numerics/Interfaces.cs` (`IMinMaxValue<T>` is in `System`, the others in
`System.Numerics`). They are the ones of .NET, with the same members and the same inheritance. A type declares only the
last interfaces of the chain (as `Int32` does in .NET):

| Types | Interfaces that they declare |
|---|---|
| `Int8`, `Int16`, `Int32`, `Int64` | `IBinaryInteger<T>`, `IMinMaxValue<T>`, `ISignedNumber<T>` |
| `UInt8`, `UInt16`, `UInt32`, `UInt64` | `IBinaryInteger<T>`, `IMinMaxValue<T>`, `IUnsignedNumber<T>` |

(`UInt8` is the Byte of .NET, `Int8` is the SByte; each of them has `IComparable<T>`, `IEquatable<T>` too.)

The chain: `IBinaryInteger<T>` is `IBinaryNumber<T>` and `IShiftOperators<T, Int32, T>`; `IBinaryNumber<T>` is
`INumber<T>` and `IBitwiseOperators<T, T, T>`; `INumber<T>` is `IComparisonOperators<T, T, Boolean>`,
`IModulusOperators<T, T, T>`, `IComparable<T>` and `INumberBase<T>`; `INumberBase<T>` is the operators `+ - * /`, unary `+ -`,
`++ --`, `==`, `IAdditiveIdentity` and `IMultiplicativeIdentity`.

## Where the members are

Each type has its members in its own file (`Int32.cs`, ...), between the lines `generic math: made by
tools/gen_numerics.py` and `end of generic math`. They are made by the script, because they are the same for the types
(they differ in the width, in the sign and in the limits): change the templates in `tools/gen_numerics.py` and run it,
do not edit the text between the marks. The script sets the lists of the interfaces in `+types.cs` too.

The members that can be written with the operators and the other members of the interface are `static virtual` in the
interface, and the types do not have them: `Clamp`, `CopySign`, `Max`, `Min`, `MaxNumber`, `MinNumber`.

An operator of a primitive type has a body that uses the operator of the language (`left + right`): the language
takes the built-in operator for the primitive types, so the operators that are declared are called through a generic
parameter only, and the code that uses the types directly is the same as before. The types that are narrower than 32
bits make the operations in `Int32` and cut the result (`(UInt8)200 + (UInt8)100` is 44).

## Tests

`test/unittest/interface/generic_math.cs` is made by `tools/gen_numerics_test.py`: it calls the interfaces with the
limits and other values for the 8 types, and the expected text is that of a model of the integers (Python numbers that
wrap to the width). `inherited_static_members.cs` is about the language (below).

## The conversions between the numbers

`T.CreateChecked(value)`, `T.CreateSaturating(value)` and `T.CreateTruncating(value)` convert a number of another type:

* checked: `OverflowException` when the value does not fit;
* saturating: the lowest or the highest value of `T` when it does not fit;
* truncating: the low bits of the value (the two's complement).

They are `static virtual` in `INumberBase<T>` and call `TryConvertFrom*` of `T`, then `TryConvertTo*` of the other type;
`NotSupportedException` when neither knows the other. The 8 integer types convert from each other: a number is read as its
sign and its magnitude (a `UInt64`) by `Numerics.IntegerConversions`, so the code is not a table of the pairs of types. A new
type that is converted from or to the integers implements `TryConvertFrom*` and `TryConvertTo*` (the integers read it through
its `TryConvertTo*`). The test is `generic_math_convert.cs` (the 64 pairs, made by `tools/gen_numerics_test.py`).

## The checked operators

`operator checked +` (also `-`, `*`, `/`, unary `-`, `++`, `--`) declares the operator that is used in a checked context
(`checked(a + b)`, a `checked { }` block, a project that is checked). Its method is `op_Checked...`, so that a type has both. When
a type has no checked operator of its own, the operator that is not checked is used. In `IAdditionOperators<T, T, T>` and the
others the checked operator is `static virtual` (it calls the operator that is not checked); the integer types declare their
own, that raise `OverflowException` (`tools/gen_numerics.py`). The test is `generic_math_checked.cs` (the 8 types, made by
`tools/gen_numerics_test.py`), and `operator_checked.cs` is about the language.

The conversions that a type declares work the same way (see below): `explicit operator checked T(S s)` is the one that a 
checked context uses.

## User conversions

A type declares the conversion from or to itself with `implicit operator`, `explicit operator`, and `explicit operator checked`:

```csharp
struct Meters
{
    public Int32 V;
    public static explicit operator Int32(Meters m) => m.V;
    public static explicit operator checked Int32(Meters m) => checked(m.V * 1000);
    public static implicit operator Meters(Int32 v) { ... }
}
```

* The methods are `op_Implicit`, `op_Explicit` and `op_CheckedExplicit`; one parameter, `public static`, and the type of the
  parameter or the type that is made is the type that declares it. A type can have several conversions from the same type to
  different types (they differ in what they make).
* `(T)x` takes a conversion of the types (the checked one in a checked context, when there is), an implicit one too.
* An implicit operator is used where a value is converted without a cast: the initial value of a variable, an assignment, the
  value of a `return`, and the arguments of a call of a method (the conversion is made by a cast that the analysis inserts).
* The arguments of a constructor (`new T(x)`, `: base(x)`, `: this(x)`) take the implicit conversions too, and so do the operands
  of the operators of the language (`five + 1`, `five == 5`, `-five`), when the operator is not defined for the type.
* An interface declares the conversions as `static abstract` (`static abstract explicit operator TSelf(Int32 v)`); `(T)x`
  and an implicit conversion of a value of `T` work through a generic parameter that has it as a constraint.
* The test is `class/conversion_operators.cs` (and `interface/generic_conversions.cs`); the errors of the declarations are
  tested in `errors/conversion_*.cs`.
* The items of a tuple can be custom structs (`Tuple<S, S>`): read, passed on, boxed by `Get`, and used through a generic
  parameter (`custom_struct/tuple_items.cs`).

## What is not here yet

* The unsigned right shift `>>>`: the language does not have it, so `IShiftOperators` has `<<` and `>>` only.
* `IFormattable`, `IParsable<T>`, `IConvertible`, `ISpanFormattable`, `ISpanParsable<T>`, `IUtf8SpanFormattable`,
  `IUtf8SpanParsable<T>`: the library has no `IFormatProvider`, no spans, no formatting and parsing of numbers.
* `IBinaryInteger<T>`: `ReadBigEndian`, `ReadLittleEndian`, `WriteBigEndian`, `WriteLittleEndian`, `TryWrite*`: they use spans.
* `Single`, `Double`, `Char` (the floating point interfaces, and the arithmetic of `Char`), and the types that the language
  does not have (`Half`, `Int128`, `Decimal`, `nint`).

## Language fixes that this needed

* The static members of the interfaces that a constraint inherits are members of the generic parameter: with
  `where T : IBinaryInteger<T>`, `T.Zero`, `a + b` and `T.Abs(a)` are found in `INumberBase<T>` and the others.
  (`generic_param_t::get_member` looked in the interface of the constraint only.)
* A static abstract or virtual method of an interface can have its own generic parameters (`Create<TOther>`), used
  through a generic parameter (`T.Create(value)`): the runtime took the arguments of the method for those of the interface,
  and did not make the method of the type for them (`resolve_constrained_static`).
* `operator checked` can be declared and is used in a checked context (the parser, `__checked_operator_method`).
* A method of a generic type is found by a reference whose types are those of the method that calls (`t.Item2` of a
  `Tuple<T, T>` in a generic method): the types are read by the caller when the host type does not know them, and
  the method that is found is not kept for the other callers (`get_method`).
* The value that a call returns, when it is an argument of another call (`Add(Add(a, b), b)`, `a + b + c` for a struct
  through a generic parameter) is made in the place of the argument; it is not copied to the destination of the caller
  of the method (`type_mark_t::argument`).
* The operand of an operator that is a method is copied like an argument of a call, and the parentheses that hold a call
  do not make its result unused: `(a >> 2).V`, `(a + b).Twice()` for a struct.
* `implicit operator`, `explicit operator` and `explicit operator checked` can be declared and are used by casts, assignments,
  initial values, returns and the arguments of calls (`find_user_conversion`, `insert_user_conversion`); the check of the
  duplicates takes the type that a conversion makes in account.
* `operator >>` can be declared (the token that follows `>>`, the second angle of two right angles, was taken as a part of
  the operator).

## Known limits that the work met

* A tuple that has a custom struct as an item (`Tuple<S, S>`, with or without a generic parameter) does not keep it: the item
  is read as zero in the interpreter, and the native programs do not make it ("tuple item of a struct"). The items that are
  numbers, strings or classes, also through a generic parameter (`T.DivRem(a, b).Item1`), are good.
