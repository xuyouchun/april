# Tuples: literals, types with names, deconstruction

A tuple is `Tuple<T1, T2, ...>` (a struct) or `ITuple<T1, T2, ...>` (a reference, made in the heap). The items are `Item1`, `Item2`..., or the
names that the declaration gives them.

## Literals

`(1, "2")` makes a tuple of two or more items. What it makes depends on the type that the context wants:

| Context | What is made |
| --- | --- |
| `Tuple<A, B>` (a struct), `Tuple<A, B>?` | `new Tuple<A, B>(1, "2")` |
| `ITuple<A, B>` | `ITuple.Create<A, B>(1, "2")`: made in the heap, never boxed |
| `Object`, `ITuple` and the other reference types | `ITuple.Create<..>(..)` with the types of the items |
| it is not known (`var t = (1, "2");`) | `new Tuple<Int32, String>(1, "2")` |

The context is the declared type of the variable that the literal is assigned to, a cast, the param of the method that is called
(the overload is found by the types of the items, then the literal is made for the type of the param), the return type of the
method and the type of the field. The items are made for the types of the tuple type, so `Tuple<Int64, String> t = (1, "2");`
converts the 1, and a literal that is an item of a literal is made for the type of the item:

```cs
MyTuple<Int32, String> n = ((1, "one"), (2, "two"), (3, "three"));
ITuple<Int32, String> c = (1, "two");
Console.WriteLine(Take((3, "t")));         // the param is an ITuple<Int32, String>
```

A literal that has an item without a type (`var n = (1, null);`) is an error (AP3218); the count of the items must be that of the type
(AP3219).

The items can have names: `(Number: 1, Str: "two")`. A `var` that is given it has the names.

## Types

`(Int32, String)` is `Tuple<Int32, String>`; each item can have a name, `(Int32 Number, String Str)`. It is a type name as the
others: a variable, a field, a property, a param, a return type, a generic argument, nullable (`(Int32, String)?`), a typedef.

```cs
typedef MyTuple = (Int32 Number, String Str);
typedef Pair<T> = (T First, T Second);

MyTuple t = (1, "2");
Console.WriteLine(t.Number + t.Str);
```

The names are those of the declaration, they are not part of the type: `(Int32 A, Int32 B)` and `(Int32 X, Int32 Y)` are the same
type, and a value is given to a variable of the other one. The names must differ, and they are not `GetSize`, `Get`, `Create`, or
`ItemN` of an item that is not the N-th (AP3220).

An item that has a name has only the name: `t.Number`, not `t.Item1` (AP3206). An item that has no name is `ItemN`.

The names are found for the variables, the fields, the properties, the params, the return values of the methods that are called, the
casts and the typedefs, and they are in the metadata of a library (a program that uses it reads `Shape.Make(7).Label`).

The names are kept in the generic arguments, and in the element of an array: the member of a generic type that is declared with a
param of the type gives the names of the argument of the object it is read from.

```cs
List<(Int32 A, String B)> list = ...;
Console.WriteLine(list[0].A);                  // an indexer
foreach (var item in list) Console.WriteLine(item.B);    // the var of a foreach
var first = list[0];                           // a var that is given what has the names
Dictionary<String, (Int32 Lo, Int32 Hi)> map = ...;
Console.WriteLine(map["a"].Hi);
foreach (var pair in map) Console.WriteLine(pair.Value.Lo);   // KeyValuePair<K, V>
(Int32 Code, String Text)[] codes = ...;
Console.WriteLine(codes[0].Text);
```

It works for `List<T>`, `Dictionary<K, V>`, arrays, and the typedefs of them (`typedef Rows = List<(Int32 A, Int32 B)>`, and
`typedef Box<T> = List<(T Key, String Text)>`, where `Box<Int32>` tells the T). `var` has the names of what it is given: a literal
that has names, a variable, a member, an indexer, a method that is called, a cast.

The names are not found when the type is made by the inference of a generic method (`list.Select(x => x)`), for the param of a
lambda that has no type, and for a member that is declared by a param of a base type of the generic type (the param of the base
is not that of the object). There the items are `Item1`, `Item2`.

## Deconstruction

`(Int32 Number, String Str) = expr;` declares the variables `Number` and `Str`, of the items of the tuple (a literal, or any
tuple, `Tuple` or `ITuple`); the type of an item is converted to that of its variable, and `var` takes the type of the item.

```cs
(Int32 Number, String Str) = (1, "2");
(Int64 big, var text) = tuple1;
(var x, var y) = (7, 8);
```

`(a, b) = (b, a);` assigns the variables that exist; the value is made before the first one is assigned, so it swaps them.
A variable that is declared has a name (AP0100); a deconstruction that has more variables than the tuple has items is an error
(`Item3` of a two items tuple, AP3206).
