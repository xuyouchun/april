# typedef: names of types

`typedef Name = Type;` gives another name to a type. The name **is** the type, not a new one: `typeof(Name)` is the type, the
overloads and the conversions are those of the type, it can be a base class, it makes objects (`new Name()`) and has the
static members (`Name.Member`).

```cs
typedef Num = Int32;
typedef Names = List<String>;
typedef Ints = Int32[];
typedef Lower = Base;                      // class Derived : Lower { }
```

The type is written with the names that are seen where the typedef is declared (the namespaces that are used, the other
typedefs): the order of the typedefs does not matter, a typedef that refers to itself (`typedef A = B; typedef B = A;`) is an error.

`using` is not for types: `using Name = A.B;` is the alias of a **namespace** (`Name.Type` is `A.B.Type`), and it is an error
when `A.B` is a type (AP3115: write `typedef Name = A.B;`).

## Where a typedef is, and who sees it

| Where | Access | Seen by |
| --- | --- | --- |
| a namespace | none, or `internal` | the assembly, by `Namespace.Name` or `using Namespace;` |
| a namespace | `public` | the assemblies that use it too (it is in the metadata) |
| a type | any access (`private` if there is none) | as the members of the type: `Type.Name` |
| a method | – | the block, from its place to the end of it (two methods can have a typedef of the same name) |

A typedef is a member: in a namespace it is that of the namespace (`private` is an error there), in a type it has the access that
it is given. It takes the access and nothing else (`static typedef ...` is an error). A typedef and a type cannot have the same
name and the same count of generic params in the same place.

## Generic typedefs

The params are written after the name and replaced by the types that are written where the typedef is used.

```cs
typedef Func<TRet, TArg> = Delegate<TRet, TArg, TArg>;    // Func<Int32, String> is Delegate<Int32, String, String>
typedef M<T> = List<T>;
typedef M<K, V> = Dictionary<K, V>;                        // M<T> and M<K, V> are two typedefs (the count of params tells)
typedef Pair<T> = Dictionary<T, T>;
```

Nested types of generic types are named with the params of the host and of the nested type:

```cs
typedef Class3<T1, T2, T3> = GenericClass3<T1>.NestClass<T2, T3>;
typedef Class4<T1> = GenericClass4<Int32>.NestClass<T1, Int64>;
```

A typedef with no params and one with params can have the same name (`M`, `M<T>`). A param that the type does not use is
warned (AP4011). The typedef of a param (`typedef Same<T> = T;`) is an error.

### Packs

The last param can be a pack (`TArgs...`): it is all the arguments that follow the others, none too. The typedef that has as many
params and no pack wins for its count; between the ones with a pack, the one that has the most params before it.

```cs
typedef Func<TRet, TArgs...> = Delegate<TRet, TArgs...>;     // Func<Int32>, Func<Int32, String>, Func<Int32, String, Int64> ...
typedef Action<TArgs...> = Delegate<Void, TArgs...>;         // Action, Action<Int32>, ...
```

`Action<TArgs...>` and `Func<TRet, TArgs...>` are in the library (`System`).

## Constraints

A param has the constraints of the generic params that it is the argument of, in every place where it is used: it needs not
write `where`. They are checked where the typedef is used, as the ones of the type are (`SBox<String>` of
`typedef SBox<T> = Box<T>;`, `Box<T> where T : struct`, is an error).

The constraints that a param gets from the places where it is used are also checked **where the typedef is declared**: a typedef
that no type can satisfy is an error (AP3113), with the constraints that are in conflict and where they come from:

* a value type (`struct`, `unmanaged`) and a reference type (`class`);
* a value type and a class that it must derive from;
* two classes that are not one a base of the other (`Base<Int32>` and `Base<String>` too);
* a class that is sealed (it is the only type that can be it) and an interface that it does not implement, or a constructor
  that it has not.

```cs
class Two<A, B> where A : struct where B : class { }
typedef Same<T> = Two<T, T>;       // error: T must be a value type (by Two.A) and a reference type (by Two.B)
```

A type that is written in the typedef and does not satisfy the constraint of its place (`typedef Bad = Box<String>;`) is an error there.
