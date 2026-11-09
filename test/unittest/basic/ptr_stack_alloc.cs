// NativeMemory.StackAlloc<T>(count): memory for the values that lives while the method runs; the memory that is kept by the
// pointers is not collected while they are used (a lot of garbage is made between the uses).

/* EXPECT

ints 0 10 20 30 40 100
doubles 2.5 5 7.5
points 3 9
loop 4950
garbage 4950 499500
sized 24 24

*/

import System;

using System;
using System.Runtime;

struct Point
{
    public Int32 X;
    public Int32 Y;
}

class Program
{
    static Int32 Fill(Ptr<Int32> p, Int32 count)
    {
        Int32 total = 0;
        for (Int32 i = 0; i < count; i++)
        {
            p[i] = i * 10;
            total += p[i];
        }

        return total;
    }

    // Garbage: arrays that nobody keeps.
    static Int64 MakeGarbage()
    {
        Int64 total = 0;
        for (Int32 i = 0; i < 2000; i++)
        {
            Int32[] junk = new Int32[1000];
            junk[i % 1000] = i;
            total += junk[i % 1000];
        }

        return total;
    }

    static Int32 Sum(Int32 count)
    {
        Ptr<Int32> p = NativeMemory.StackAlloc<Int32>(count);
        for (Int32 i = 0; i < count; i++)
            p[i] = i;

        // The collector runs between the writes and the reads.
        MakeGarbage();

        Int32 total = 0;
        for (Int32 i = 0; i < count; i++)
            total += p[i];

        return total;
    }

    static Int64 Collected(Int32 count)
    {
        Ptr<Int64> p = NativeMemory.StackAlloc<Int64>(count);
        for (Int32 i = 0; i < count; i++)
            p[i] = i;

        MakeGarbage();

        Int64 total = 0;
        for (Int32 i = 0; i < count; i++)
            total += p[i];

        return total;
    }

    [EntryPoint]
    public static void Main()
    {
        Ptr<Int32> a = NativeMemory.StackAlloc<Int32>(5);
        Int32 total = Fill(a, 5);
        Console.WriteLine("ints " + a[0] + " " + a[1] + " " + a[2] + " " + a[3] + " " + a[4] + " " + total);

        Ptr<Double> d = NativeMemory.StackAlloc<Double>(3);
        d[0] = 2.5; d[1] = 5.0; d[2] = 7.5;
        Console.WriteLine("doubles " + d[0] + " " + d[1] + " " + d[2]);

        Ptr<Point> points = NativeMemory.StackAlloc<Point>(2);
        Point p = new Point();
        p.X = 1; p.Y = 2;
        points[0] = p;
        p.X = 3; p.Y = 4;
        points[1] = p;
        Console.WriteLine("points " + (points[0].X + points[0].Y) + " " + (points[1].X + points[1].Y + 0 * points[0].X + 2));

        // Many allocations of one place: each takes a buffer.
        Int32 sum = 0;
        for (Int32 i = 0; i < 100; i++)
        {
            Ptr<Int32> cell = NativeMemory.StackAlloc<Int32>(1);
            cell.Value = i;
            sum += cell.Value;
        }

        Console.WriteLine("loop " + sum);

        Console.WriteLine("garbage " + Sum(100) + " " + Collected(1000));

        Console.WriteLine("sized " + (NativeMemory.SizeOf<Int64>() * 3) + " " + NativeMemory.SizeOf<Point>() * 3);
    }
}
