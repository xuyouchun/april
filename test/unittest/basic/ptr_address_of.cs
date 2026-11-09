// Ptr.AddressOf: the address of an array (a value of it), of a string, of a variable, a field and an element (ref);
// NativeMemory.Copy and Fill.

/* EXPECT

array 1 1 2 3 4
element 3 30 3
write 100 2 300
chars 3 a b c 0
string-index 98 98
ref 7 8 9 11 12
copy 1 2 3 1 2 3
fill 255 255 255 0
temp 6
empty caught
bad index caught

*/

import System;

using System;
using System.Runtime;

class Holder
{
    public Int32 Count;
    public Int32[] Values = new Int32[3];
}

class Program
{
    static Int32 SumOf(Int32[] values)
    {
        Ptr<Int32> p = Ptr.AddressOf(values);
        Int32 total = 0;
        for (Int32 i = 0; i < values.Length; i++)
            total += p[i];

        return total;
    }

    [EntryPoint]
    public static void Main()
    {
        Int32[] a = new Int32[] { 1, 2, 3, 4 };
        Ptr<Int32> p = Ptr.AddressOf(a);
        Console.WriteLine("array " + p.Value + " " + p[0] + " " + p[1] + " " + p[2] + " " + p[3]);

        Ptr<Int32> q = Ptr.AddressOf(a, 2);
        Console.WriteLine("element " + q.Value + " " + (q.Value * 10) + " " + (q - p + 1));

        // It writes the array.
        p.Value = 100;
        q.Value = 300;
        Console.WriteLine("write " + a[0] + " " + a[1] + " " + a[2]);

        String s = "abc";
        Ptr<Char> c = Ptr.AddressOf(s);
        Console.WriteLine("chars " + s.Length + " " + c[0] + " " + c[1] + " " + c[2] + " " + (Int32)c[3]);

        Char[] letters = new Char[] { 'x', 'c', 'd' };
        Ptr<Char> d = Ptr.AddressOf(s, 1);
        Console.WriteLine("string-index " + (Int32)d[0] + " " + (Int32)d.Value);

        // ref: a variable, a field, an element.
        Int32 n = 7;
        Holder h = new Holder();
        h.Count = 8;
        h.Values[1] = 9;
        Ptr<Int32> pn = Ptr.AddressOf(ref n);
        Ptr<Int32> pc = Ptr.AddressOf(ref h.Count);
        Ptr<Int32> pv = Ptr.AddressOf(ref h.Values[1]);
        Console.WriteLine("ref " + pn.Value + " " + pc.Value + " " + pv.Value + " " + (pn.Value + 4) + " " + (pc.Value + 4));

        // Copy and fill.
        Int32[] source = new Int32[] { 1, 2, 3 };
        Int32[] target = new Int32[3];
        NativeMemory.Copy(Ptr.AddressOf(target).ToPtr(), Ptr.AddressOf(source).ToPtr(), 12);
        Console.WriteLine("copy " + source[0] + " " + source[1] + " " + source[2] + " " + target[0] + " " + target[1] + " " + target[2]);

        UInt8[] bytes = new UInt8[4];
        NativeMemory.Fill(Ptr.AddressOf(bytes).ToPtr(), (UInt8)255, 3);
        Console.WriteLine("fill " + bytes[0] + " " + bytes[1] + " " + bytes[2] + " " + bytes[3]);

        // The array is a value of an expression.
        Console.WriteLine("temp " + SumOf(new Int32[] { 1, 2, 3 }));

        String caught = "no";
        try { Ptr.AddressOf(new Int32[0]); }
        catch (OutOfRangeException) { caught = "empty caught"; }
        Console.WriteLine(caught);

        caught = "no";
        try { Ptr.AddressOf(a, 4); }
        catch (OutOfRangeException) { caught = "bad index caught"; }
        Console.WriteLine(caught);
    }
}
