# What the compiler makes

Some code of a program is made by the compiler: the iterators (`yield`), the lambdas that use the
variables of their method, the automatic properties and the events that have no accessors. This
document tells what it makes, how it is named, how it is marked, and how the reflection shows it.

## The classes: nest types

The state of an iterator and the environment of a lambda (the object that has the values of the
variables that the lambda uses) are classes that are **nested in the type that has the method**.
They are not seen in the namespace. A type that is generic has them with its generic parameters
(`Bag<T>` has the state of its iterators, an instance `Bag<Int32>` has the one of `Bag<Int32>`).
The lambdas are methods of the type that has the method; the part of an iterator that does the
work is a method of it too.

`Type.GetNestedTypes()` lists them, after the types that the user wrote.

## Names

Every name that the compiler makes is

```
__ap_<usage>_<detail>_<hash>
```

* `usage` tells what it is for: `iterator`, `move`, `lambda`, `env` (the classes and the methods),
  `state`, `current`, `delegate`, `finish`, `source`, `host`, `local`, `captured` (the members of the
  state and of the environment), `backing` (the field of an automatic property or of an event),
  `temp`, `foreach_array`, `foreach_index`, `using`, `catch` (the variables of the method).
* `detail` is the serial of the thing among those of the same usage of the method (the first lambda
  of a method is 0), or the name of what it is made for (`__ap_captured_<variable>_<hash>`,
  `__ap_backing_<Name>_<hash>`). It is empty for some of them (`__ap_state_<hash>`).
* `hash` is 6 hexadecimal digits (FNV-1a, the same on every platform) of the usage, the owner and
  the detail. The owner of a class or of a method is the full name of its type and the method with
  the types of its parameters, so the overloads have their own names; the owner of a member is the
  class that has it.

The same code gives the same names every time it is compiled, and what is made for two methods does
not have the same name. A name that a user writes is not told from them: do not begin a name with
`__ap_`.

## The attribute

`System.Runtime.CompilerServices.CompilerGeneratedAttribute` is on all of it: the classes and their
members, the lambdas, the methods that do the work of the iterators, the backing fields, the
accessors and the automatic properties, the accessors and the backing fields of the events, and the
default constructors. It is an attribute like the others: `GetAttributes()` gives it, and the
reflection does not treat what has it in another way (`GetFields()`, `GetMethods()`, `GetField(name)`
... list and find it).

## Reflection of the arguments of the attributes

The arguments of an attribute can be numbers, strings, `null`, enums, and `typeof(T)`. A parameter
(or a named field) of type `Object` gets a box of the number or the string in the type that was
written (`5` is an `Int32`, `2.5` a `Double`, `true` a `Boolean`); one of type `Type` gets the type.
An array of numbers or of strings is an argument too, as a parameter or as a named field, of any
dimension: `new Int32[] { 1, 2 }`, `new Int32[,] { { 1, 2 }, { 3, 4 } }`. An empty one is given by an
empty initializer (`new Int32[] { }`) or by lengths of which one is 0 (`new Int32[0]`, `new Int32[0, 3]`).
The elements and the lengths are constants (at most 511 of them with the lengths), the other lengths
of an array that is not empty are not an argument.

## The params of an iterator

The state object keeps the params of the method, so an iterator takes the same params as any other
method, but `ref` and `out` ones (`AP5205`: they cannot be kept). A `params` array is kept as the array.
The items of a params generic parameter (`Pick<T ...>(T ... items)`) are kept as a tuple
(`ITuple<T ...>`): the code of the iterator gives them to other methods (`F(items ...)`), as in any
generic method. The method that does the work takes them again from the tuple, with the arguments
of the call written as `tuple ...`, that the compiler makes (`tuple ...` is a way to give the items
of a tuple of a params generic parameter as the arguments of a call).

## Limits

* The iterator of a method that has a `ref` or `out` param is not supported (`AP5205`).
* The tuple that keeps the items of a params generic parameter cannot have items that are custom
  structs in the native back end.

## The ToString of an enum

An enum that does not declare a `ToString()` gets one that is made by the compiler: it returns the
name of the item that the value is (`Color.Green.ToString()` is `"Green"`), and the number when the
value has no item (a number that is cast to the enum, a combination of items: `"9"`, `"3"`). An enum
that declares its own `ToString()` keeps it. The text of a boxed enum (`"" + color`,
`Object.ToString(obj)`) is the one of this method, read by reflection. The number is made by
`Enum.__Number`, a method of the core library. The method is a member of the enum, public, not marked.

## The temporary variables

The temporaries of an expression (the struct that a property or a call returns and whose member is
read, the array that an initializer fills) are local variables that are named `temp`. They are marked
in the assembly (the highest bit of the index of the variable) and the loader gives each of them a
place of its own in the frame, apart from the places that the variables of the blocks share.
