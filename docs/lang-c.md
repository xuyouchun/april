# The language c

`c` is a language that is embedded in the code of another language (`cs`, like `lq`): a statement with the pointers of C.
It has no keyword `unsafe`: the pointers are the types `System.Ptr<T>` and `System.Ptr` (see `pointers.md`), and the syntax of
`c` is only a way to write them.

```csharp
Int32 total = 0;

<%c
{
    int* values = stackalloc int[10];
    for (int i = 0; i < 10; i++)
        values[i] = i * i;

    for (int* p = values; p < values + 10; p++)
        total += *p;
}
%>

Console.WriteLine(total);       // 285
```

## Where the code of c is

* `<%c ... %>`: the code is one statement (a block `{ ... }` has as many as it needs).
* `` ``c statement `` : the code is the statement that follows it, up to its end.

The code is a statement of the method that has it: the variables of the method are the ones of the code (read and written),
what the code declares is in its block, as in a block of the language that has it. `return`, `break` and `continue` are those of
the method and of its loops. No class, no method, no namespace in `c`.

## The syntax

| In `c` | It is |
|---|---|
| `int* p`, `Point* q`, `void* r`, `int** pp` | `Ptr<Int32>`, `Ptr<Point>`, `Ptr`, `Ptr<Ptr<Int32>>` (the types of a variable and of a cast) |
| `*p`, `*p = v`, `*p += v`, `*p++` | `p.Value` (`*p++` is `(p++).Value`, as in C) |
| `p[i]` | `p[i]` of `Ptr<T>` |
| `p->f` | `p.Value.f` |
| `&x`, `&a[i]`, `&o.f`, `&*p`, `&p[i]` | `System.Ptr.AddressOf(ref x)`; of `*p` and `p[i]`: `p`, `p + i` |
| `(int*)q`, `*(int*)raw` | the cast of a pointer, the value of it |
| `p + n`, `p - n`, `p - q`, `p++`, `p--`, `p += n`, `p < q` | the operators of `Ptr<T>` (`void*`: bytes) |
| `sizeof(T)` | `NativeMemory.SizeOf<T>()` |
| `stackalloc T[n]` | `NativeMemory.StackAlloc<T>(n)` |

The rest of the code is the code of the language that has it: the statements (`if`, `for`, `while`, `switch`, the definitions, the
calls, `try`), the expressions, the types of the language.

`T*` as a type is in the definition of a variable (`int* p = ...;`, `for (int* p = ...; ...)`) and in a cast. Elsewhere (a generic
argument, a parameter) write `Ptr<T>`.

## What is different from C

* There is no `unsafe`, no `fixed`: a pointer to an array or to a variable is `&a[0]`, `&x`; the compiler keeps the array alive
  while the method runs, and it does not let such a pointer be stored in a field or returned (see `pointers.md`).
* `*p` of a pointer to a number, a `Char`, a `Boolean` or a struct; the value that `p` points to is not checked (`*null` is a
  crash, as in C).
* `*p++` is `*(p++)`; `++` and `--` of a value are part of it (`a[i]++`, `p->x++`).
* `p->f` is `p.Value.f` (a pointer to a struct).
* `Point* p; ` is a definition of a pointer, not a product: a statement that starts with a name and a star, and a name, is one.

## How it is made

The patterns of the grammar are those of `cs` (`modules/lang_cs/text.src`), changed where they are and added to by
`modules/lang_c/text.src` (the rules `$c_deref`, `$c_address`, `$c_pointer_type_name`, `$c_sizeof`, `$c_stackalloc`, and the root
`$: $c_stmt code_end?`, `code_end` is the token that the analyzer adds after the code of an embedded language). The nodes that the builders make (`modules/lang_c/c_ast.cpp`) are those of `cs`: `*p` is a simple
expression that has `p`, `.`, `Value`; `&x` a call of `System.Ptr.AddressOf` with a `ref` argument; `T*` the type name
`System.Ptr<T>`. The code of `c` is not compiled by anything of its own: the compiler of `cs` takes its nodes.
