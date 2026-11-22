// Array<T>: the items of an array of any type, structs too, are read and written through the base type (Array<T>.this[]), also
// of a multi-dimensional array (the index is the linear one).

/* EXPECT

get 5 6
set 7 8
int 9 4
str xy
class 3
grid 11 6
double 2.5
range True

*/

import System;
using System;
struct S { public Int32 A; public Int32 B; }
class C { public Int32 N; }
class Program {
    [EntryPoint]
    public static void Main() {
        S[] items = new S[3];
        items[2].A = 5; items[2].B = 6;
        Array<S> arr = items;
        S s = arr[2];
        Console.WriteLine("get " + s.A + " " + s.B);
        S t = new S(); t.A = 7; t.B = 8;
        arr[1] = t;
        Console.WriteLine("set " + items[1].A + " " + items[1].B);
        Int32[] ints = new Int32[2]; ints[1] = 9;
        Array<Int32> ai = ints; ai[0] = 4;
        Console.WriteLine("int " + ai[1] + " " + ints[0]);
        String[] strs = new String[2]; strs[1] = "x";
        Array<String> ast = strs; ast[0] = "y";
        Console.WriteLine("str " + ast[1] + ast[0]);
        C[] cs = new C[1]; cs[0] = new C(); cs[0].N = 3;
        Array<C> ac = cs;
        Console.WriteLine("class " + ac[0].N);
        Int32[,] grid = new Int32[2, 3]; grid[1, 2] = 11;
        Array<Int32> ag = grid;
        Console.WriteLine("grid " + ag[5] + " " + ag.Length);
        Double[] ds = new Double[2]; ds[1] = 2.5; Array<Double> ad = ds; Console.WriteLine("double " + ad[1]);
        Boolean oob = false;
        try { S z = arr[5]; } catch (OutOfRangeException e) { oob = true; }
        Console.WriteLine("range " + oob);
    }
}
