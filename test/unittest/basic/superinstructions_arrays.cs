// The superinstructions of the arrays (modules/exec/fusion.h, array_load and array_store): the index and the array may be a local, an
// argument or a constant, or be in the stack; the elements of every type; the jump that ends a loop after a store; the exceptions
// (null array, index out of range) that go to the block of the catch; the arrays of more dimensions are not changed. The results are
// those of the commands one by one (APRIL_SUPER=off), whatever the mode is.

/* EXPECT

ints 4950 9900 99
types 1 2 3 4 5 6 7 8 1.5 2.5 x True
stack 10 60 60
nested 1 9
const 7 70
args 5 50
loop 49995000 10000
null caught caught
range caught caught caught 5
objects 3 b True
dims 6 33
tier 10652124

*/

import System;

using System;

class Item
{
    public Int32 Id;

    public Item(Int32 id) { Id = id; }
}

class Program
{
    static Int32 Sum(Int32[] a)
    {
        Int32 s = 0;
        for (Int32 i = 0; i < a.Length; i++)
            s += a[i];

        return s;
    }

    static Int32 Read(Int32[] a, Int32 i) { return a[i]; }

    static void Write(Int32[] a, Int32 i, Int32 v) { a[i] = v; }

    static Int32 Second(Int32[] a) { return a[1]; }

    static void SetSecond(Int32[] a, Int32 v) { a[1] = v; }

    static Int32 Tier(Int32[] a, Int32 i) { a[i & 15] = a[i & 15] + i; return a[i & 15]; }

    static Int32 NullRead(Int32[] a) { return a[0]; }

    static void NullWrite(Int32[] a) { a[0] = 1; }

    static Int32 Past(Int32[] a, Int32 i) { return a[i]; }

    [EntryPoint]
    public static void Main()
    {
        // Loops: a read of the element of a local array by a local index, a write and the jump back.
        Int32[] a = new Int32[100];
        for (Int32 i = 0; i < 100; i++)
            a[i] = i * 2;

        Int32 sum = 0;
        for (Int32 i = 0; i < 100; i++)
            sum += a[i] / 2;

        Console.WriteLine("ints " + sum + " " + Sum(a) + " " + a[99] / 2);

        // The elements of every type.
        Int8[] sb = new Int8[2]; UInt8[] by = new UInt8[2]; Int16[] i16 = new Int16[2]; UInt16[] u16 = new UInt16[2];
        UInt32[] u32 = new UInt32[2]; Int64[] i64 = new Int64[2]; UInt64[] u64 = new UInt64[2];
        Single[] f = new Single[2]; Double[] d = new Double[2]; Char[] c = new Char[2]; Boolean[] bo = new Boolean[2];
        String[] st = new String[2];

        Int32 k = 1;
        sb[k] = 1; by[k] = 2; i16[k] = 3; u16[k] = 4; u32[k] = 5; i64[k] = 6; u64[k] = 7;
        d[k] = 2.5; f[k] = 1.5f; c[k] = 'x'; bo[k] = true; st[k] = "s";

        Console.WriteLine("types " + sb[k] + " " + by[k] + " " + i16[k] + " " + u16[k] + " " + u32[k] + " " + i64[k] + " " + u64[k]
                          + " " + (sb[k] + by[k] + i16[k] + u16[k] + u32[k] + i64[k] - 13) + " " + f[k] + " " + d[k] + " " + c[k] + " "
                          + (bo[k] ? "True" : "False"));

        // The index and the value are the results of expressions (they are in the stack).
        Int32[] b = new Int32[8];
        Int32 n = 3;
        b[n + 1] = n * 10;
        b[n * 2] = b[n + 1] + 30;
        Console.WriteLine("stack " + (b[n + 1] / 3) + " " + (b[n + 1] * 2) + " " + b[6]);

        // An element that is the index of another one.
        Int32[] p = new Int32[4];
        p[0] = 2; p[1] = 3; p[2] = 1; p[3] = 0;
        Int32[] q = new Int32[4];
        q[p[0]] = p[p[1]] + 1;
        q[p[p[0]]] = 9;
        Console.WriteLine("nested " + q[2] + " " + q[1]);

        // The constants.
        Int32[] cs = new Int32[10];
        cs[3] = 7;
        cs[4] = cs[3] * 10;
        Console.WriteLine("const " + cs[3] + " " + cs[4]);

        // The arguments.
        Int32[] ar = new Int32[6];
        Write(ar, 2, 5);
        SetSecond(ar, 50);
        Console.WriteLine("args " + Read(ar, 2) + " " + Second(ar));

        // A loop of stores with the jump (a big array, the loop of the sieve).
        Boolean[] flags = new Boolean[10001];
        Int32 marked = 0;
        for (Int64 j = 0; j <= 10000; j += 1)
            flags[(Int32)j] = true;

        for (Int32 j = 0; j <= 10000; j++)
            if (flags[j])
                marked++;

        Int32 total = 0;
        Int32[] big = new Int32[10000];
        for (Int32 j = 0; j < 10000; j++)
            big[j] = j;

        for (Int32 j = 0; j < 10000; j++)
            total += big[j];

        Console.WriteLine("loop " + total + " " + (marked - 1));

        // The exceptions: a null array, an index out of range (the stack is the one of the block of the catch).
        String r1 = "none", r2 = "none";
        try { Int32 v = NullRead(null); r1 = "read " + v; }
        catch (NullReferenceException) { r1 = "caught"; }

        try { NullWrite(null); r2 = "written"; }
        catch (NullReferenceException) { r2 = "caught"; }

        Console.WriteLine("null " + r1 + " " + r2);

        Int32[] small = new Int32[5];
        String r3 = "none", r4 = "none", r5 = "none";
        try { Int32 v = Past(small, 5); r3 = "read " + v; }
        catch (OutOfRangeException) { r3 = "caught"; }

        try { Int32 v = Past(small, -1); r4 = "read " + v; }
        catch (OutOfRangeException) { r4 = "caught"; }

        try { small[7] = 3; r5 = "written"; }
        catch (OutOfRangeException) { r5 = "caught"; }

        Console.WriteLine("range " + r3 + " " + r4 + " " + r5 + " " + small.Length);

        // Objects.
        Item[] items = new Item[3];
        items[0] = new Item(1);
        items[1] = new Item(2);
        items[2] = new Item(3);
        String[] names = new String[3];
        names[0] = "a"; names[1] = "b"; names[2] = "c";
        Int32 idx = 2;
        Console.WriteLine("objects " + items[idx].Id + " " + names[1] + " " + (items[0] != null));

        // The arrays of more dimensions.
        Int32[,] grid = new Int32[3, 4];
        for (Int32 i = 0; i < 3; i++)
            for (Int32 j = 0; j < 4; j++)
                grid[i, j] = i * 4 + j;

        Console.WriteLine("dims " + grid[1, 2] + " " + (grid[2, 3] * 3));

        // A method that runs many times (it gets its commands after the 50 calls).
        Int32[] t = new Int32[16];
        Int32 last = 0;
        for (Int32 i = 0; i < 1000; i++)
            last += Tier(t, i);

        Console.WriteLine("tier " + last);
    }
}
