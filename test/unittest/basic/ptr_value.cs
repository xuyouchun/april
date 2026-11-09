// Ptr<T>: *p and p[i] (Value, the indexer), the arithmetic (+ - ++ -- += -=, the difference), the compares, over the
// memory that is not collected (NativeMemory).

/* EXPECT

values 10 20 30 40
sum 100
index 7 8 9
moved 3 4 6
steps 10 20 40
compare True True False True True
diff 4 12 2
bytes 305419896 120 18
compound 15 14 2
types 1 -2 3 -4 5 -6 7 8 1.5 2.25 x True
nested 20
postfix 7 8 7
done

*/

import System;

using System;
using System.Runtime;

class Program
{
    static Int32 Sum(Ptr<Int32> p, Int32 count)
    {
        Int32 total = 0;
        for (Int32 k = 0; k < count; k++)
        {
            total += p.Value;
            p++;
        }

        return total;
    }

    [EntryPoint]
    public static void Main()
    {
        Ptr<Int32> a = NativeMemory.Alloc(16 * 4).As<Int32>();

        a.Value = 10;
        (a + 1).Value = 20;
        a[2] = 30;
        a[3] = 40;
        Console.WriteLine("values " + a.Value + " " + a[1] + " " + (a + 2).Value + " " + a[3]);
        Console.WriteLine("sum " + Sum(a, 4));

        // Index and negative index.
        Ptr<Int32> b = a + 8;
        b[0] = 7; b[1] = 8; b[2] = 9;
        Ptr<Int32> c = b + 2;
        Console.WriteLine("index " + b[0] + " " + b[1] + " " + c[0]);

        // p - n, p -= n, the pointer that moved.
        Ptr<Int32> d = c - 2;
        Int64 moved = (c - d);
        Console.WriteLine("moved " + (d == b ? 3 : 0) + " " + (b - a) / 2 + " " + (c - a - 4));

        // ++ and --, prefix and postfix.
        Ptr<Int32> e = a;
        Console.WriteLine("steps " + e.Value + " " + (++e).Value + " " + (e++ == a + 1 ? (e).Value + 10 : 0));

        // The compares.
        Console.WriteLine("compare " + (a < b) + " " + (a <= a) + " " + (a > b) + " " + (b >= a) + " " + (a != b));

        // The differences.
        Console.WriteLine("diff " + (b - a) / 2 + " " + (Ptr.FromInt64(a.ToInt64() + 12) - a.ToPtr()) + " " + (c - b));

        // The same memory seen as bytes.
        a.Value = 0x12345678;
        Ptr<UInt8> bytes = a.As<UInt8>();
        Console.WriteLine("bytes " + a.Value + " " + bytes[0] + " " + bytes[3]);

        // Compound assignments of Value and of the indexer.
        a.Value = 10;
        a.Value += 5;
        a[1] = 20;
        a[1] -= 6;
        a[2] = 1;
        a[2]++;
        Console.WriteLine("compound " + a.Value + " " + a[1] + " " + a[2]);

        // The other types.
        Ptr<Int8> i8 = NativeMemory.Alloc(64).As<Int8>();
        i8[0] = 1; i8[1] = -2;
        Ptr<Int16> i16 = (i8 + 8).As<Int16>();
        i16[0] = 3; i16[1] = -4;
        Ptr<Int64> i64 = (i8 + 16).As<Int64>();
        i64[0] = 5; i64[1] = -6;
        Ptr<UInt16> u16 = (i8 + 40).As<UInt16>();
        u16[0] = 7;
        Ptr<UInt32> u32 = (i8 + 44).As<UInt32>();
        u32[0] = 8;
        Ptr<Single> f = (i8 + 48).As<Single>();
        f[0] = 1.5f;
        Ptr<Double> g = (i8 + 56).As<Double>();
        g[0] = 2.25;
        Ptr<Char> ch = (i8 + 32).As<Char>();
        ch[0] = 'x';
        Ptr<Boolean> bo = (i8 + 34).As<Boolean>();
        bo[0] = true;
        Console.WriteLine("types " + i8[0] + " " + i8[1] + " " + i16[0] + " " + i16[1] + " " + i64[0] + " " + i64[1] + " "
            + u16[0] + " " + u32[0] + " " + f[0] + " " + g[0] + " " + ch[0] + " " + bo[0]);

        // A pointer to pointers.
        Ptr<Ptr<Int32>> table = NativeMemory.Alloc(16).As<Ptr<Int32>>();
        table[0] = a;
        table[1] = b;
        table[1][0] = 20;
        Console.WriteLine("nested " + table[1].Value);

        // The value of x++ is the old value.
        a[1] = 7;
        Int32 u = a[1]++;
        Int32 w = a[1]--;
        Console.WriteLine("postfix " + u + " " + w + " " + a[1]);

        NativeMemory.Free(table);
        NativeMemory.Free(i8);
        NativeMemory.Free(a);
        Console.WriteLine("done");
    }
}
