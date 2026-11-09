# Pointer benchmark

The same loop (4096 values, 50000 rounds, the sum of the values) with an array, with `p[i]`, and with `p.Value; p++`, and the
same loop in C++. The result of each is 419328000000.

    april run --interpret array.cs        (use 5000 rounds: the interpreter is slower)
    april run array.cs                    (the JIT)
    april compile --native -o array.exe array.cs
    clang++ -O2 baseline.cpp

The numbers are in docs/pointers.md.
