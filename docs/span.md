# Span

`Span<T>` and `ReadOnlySpan<T>` (`core_lib/System/Span.cs`) are a window on the items of an array: a range of the items,
not a copy of them. A change of an item of a span is a change of the array and of the other spans on it; a slice is a
window on the same items. A span is a struct of 3 fields (the array, the index of the first item, the length), so it is cheap to
pass, to return and to slice. It keeps the array alive (it is a normal reference: the collector knows it).

```csharp
Int32[] items = new Int32[] { 10, 20, 30, 40, 50 };
Span<Int32> all = items;                 // the array is a span
Span<Int32> mid = all.Slice(1, 3);       // 20 30 40, the same items
mid[0] = 99;                             // items[1] is 99
ReadOnlySpan<Int32> view = mid;          // a span is a read-only span
foreach (Int32 v in mid) { ... }
```

| Member | |
|---|---|
| `new Span<T>(T[])`, `new Span<T>(T[], start, length)` | a null array is an empty span; a range that is not in the array is an `OutOfRangeException` |
| `Span<T>.Empty`, `Length`, `IsEmpty` | |
| `this[Int32]` | get and set (`ReadOnlySpan<T>`: get only); an index that is not in the span is an `OutOfRangeException` |
| `Slice(start)`, `Slice(start, length)` | |
| `ToArray()` | a new array with the items |
| `CopyTo(Span<T>)`, `TryCopyTo(Span<T>)` | the destination must have room for the items; the ranges of one array can overlap (the items are the ones that the source had) |
| `Fill(value)`, `Clear()` | `Span<T>` only |
| `GetEnumerator()` | `foreach`, with a struct enumerator (`SpanEnumerator<T>`), the items are not copied |
| `implicit` conversions | `T[]` to `Span<T>`, `T[]` to `ReadOnlySpan<T>`, `Span<T>` to `ReadOnlySpan<T>` |

## Limits

* A span is on an array: not on a string, memory of the stack or the data of an assembly. (A span that holds a pointer
  would need the collector to treat the field as an interior pointer, and new internal functions in the three ways to run
  the code: that is for later; the members above do not depend on it.)
* There is no `ref` return: `span[k].X = 1` does not change the item of a span of structs (the item is a copy); set the
  whole item (`span[k] = item`).
* The rules of a `ref struct` are not checked: a span can be a field of a class, an item of an array or a captured variable.
* `SequenceEqual`, `IndexOf` and the others that compare the items are not there.
