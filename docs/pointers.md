# Pointers: `Ptr`, `Ptr<T>`

`System.Ptr` is a pointer to nothing in particular (`void*`), `System.Ptr<T>` a pointer to a value of the unmanaged type `T`
(`T*`). Both are an address of 8 bytes, a value like a number: they are locals, fields, parameters, results, elements of
arrays, fields of structs and arguments of generics, with the same cost as an `Int64`.

There is no `unsafe` keyword, no `fixed`, no pointer syntax in the language: the members of the types are the operations.

```csharp
using System.Runtime;

Ptr<Int32> a = NativeMemory.Alloc(16 * 4).As<Int32>();

a.Value = 10;                // *a = 10
a[2] = 30;                   // a[2] = 30
Int32 x = (a + 2).Value;     // *(a + 2)
a.Value += 5;                // *a += 5
for (Ptr<Int32> p = a; p < a + 16; p++)
    p.Value = 0;

NativeMemory.Free(a);
```

## `unmanaged`

`where T : unmanaged`: the argument is a number, `Boolean`, `Char`, a pointer, an enum, or a struct whose instance fields are all
unmanaged (nested and generic structs too). A class, an array, a string, and a struct that has a reference are not. A generic
param is unmanaged when it has the constraint. `Ptr<T>` has it: `Ptr<String>` is the error AP6115. (The assemblies do not keep
constraints: the compiler checks `Ptr<>` itself.)

## Members

| | `Ptr<T>` | `Ptr` |
|---|---|---|
| `Null`, `FromInt64(n)`, `ToInt64()`, `IsNull` | yes | yes |
| `As<U>()` | `Ptr<U>` | `Ptr<U>` |
| `ToPtr()` | `Ptr` | |
| `Value` (get, set), `this[Int64]` (get, set) | the value of `T` | none |
| `p + n`, `p - n`, `p += n`, `p -= n`, `++`, `--` | by values (`n * size of T`) | by bytes |
| `p - q` | the number of values | the number of bytes |
| `==`, `!=`, `<`, `<=`, `>`, `>=` | the addresses, as unsigned numbers | the same |

The conversions: `Ptr<T>` is converted to `Ptr` implicitly; the others need a cast (`(Ptr<Int32>)p`, `p.As<Int32>()`): a
`Ptr<Int32>` is not a `Ptr<Double>` by itself.

## How they are compiled

Every operation of the table is one instruction, or a few, in the interpreter, the JIT and the native programs: there is no call
(the bodies in `core_lib/System/Ptr.cs` are never executed).

* `p.Value`, `p[i]`: the storage `ptr_content` of the xil: push reads the value at the address that is at the top of the
  stack, pop writes the value that is at the top to the address that is below it. An access is a load or a store with
  `align 1`; the address is not checked (null too), as in C.
* `p + n`: `address + n * size` with the arithmetic of 64 bits and a conversion between the pointer and the integer (the same
  bits).
* The compares are those of unsigned 64 bits numbers.

## Taking the address: `Ptr.AddressOf`

```csharp
Int32[] values = new Int32[16];
Ptr<Int32> first = Ptr.AddressOf(values);          // the first value (an array that has none is an OutOfRangeException)
Ptr<Int32> third = Ptr.AddressOf(values, 2);       // the index is checked
Ptr<Char>  chars = Ptr.AddressOf("text");          // the characters of a string, to read (it must be alive)
Ptr<Int32> field = Ptr.AddressOf(ref holder.Count); // a variable, a field, an element: the address of a ref argument
```

`ref` arguments can be `o.f` and `a[i]` now (not only a variable). The array and `ref` forms are made by the compiler, no call;
the array is kept by a variable of the method, so it is not collected while the method runs.

A pointer to the memory of the method (`AddressOf` of an array or of a `ref`, `NativeMemory.StackAlloc`, and what is made of
them by `p + n`, `As<T>()`, `ToPtr()`, a cast) can be held by a local variable and passed as an argument, but it is not stored
in a field, an element or a property, and it is not returned: the compiler says so ("the pointer ... is to the memory of the
method"). A pointer that must live longer is that of `NativeMemory.Alloc` (or of a resource).

## Memory: `System.Runtime.NativeMemory`

| | |
|---|---|
| `Alloc(bytes)`, `Alloc<T>(count)` | memory that is not collected (not cleared); `Free(p)` gives it back |
| `StackAlloc<T>(count)` | memory that lives while the method runs; the content is not defined |
| `SizeOf<T>()` | the size of a value of `T` (a generic param is the type that it is when the code runs) |
| `Copy(dst, src, bytes)`, `CopyValues<T>(dst, src, count)`, `Fill(dst, byte, bytes)` | the ranges may overlap |
| `Resource.GetPtr(name)` | the bytes of a resource, not copied (`Resource.GetSize` tells how many) |

`StackAlloc` is a buffer of bytes that a variable of the method keeps, in the three back ends (not the instruction `alloca`
yet).

## Structs

`Ptr<Point>` works: `p.Value = s`, `Point q = p[i]`, `p.Value.X = 1`, `p[i].X` read and write the value that is in the memory
(the layout is the one of the runtime: the fields are in the order of their size). `p + n` moves by the size of the struct.

## Limits

* The value that a `Ptr<T>` points to, where `T` is a generic param of the method (`T Get<T>(Ptr<T> p) => p.Value`), is not
  supported yet (the compiler says so). The arithmetic (`p + n`, `p - q`) and `SizeOf<T>()` of a generic param are.
* `Ptr.AddressOf(String)` does not keep the string alive (it must be in a variable while the pointer is used).
* The collector does not move the objects; the pointers to arrays are valid as long as the method runs (the array is kept).

## Speed

200 millions reads of an `Int32` (4096 values, 50000 rounds; the time of the process, in seconds). The same program with an
array (`a[i]`, with the check of the index), with `p[i]` and with `*p++` (`p.Value`, `p++`).

| | array | `p[i]` | `p.Value; p++` | C++ `-O2` (no vectorization) |
|---|---|---|---|---|
| JIT | 0.77 | 0.41 | 0.47 | 0.34 - 0.54 |
| native (exe) | 0.96 | 1.00 | 1.18 | |
| interpreter (5000 rounds) | 0.88 | 1.05 | 0.91 | |

The pointers are not slower than the arrays, and in the JIT they are faster (no check of the index). The native executables
are as slow as for the arrays: the optimization level of `april compile --native` does not change the time (to be seen).
