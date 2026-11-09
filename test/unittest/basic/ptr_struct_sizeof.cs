// NativeMemory.SizeOf<T>, Alloc<T>, Copy<T>; pointers to structs (p.Value, p[i], p.Value.X, p[i].X, p + n by the size of
// the struct); the size of a generic param in the arithmetic of a pointer.

/* EXPECT

sizes 1 2 4 8 4 8 8 8
struct 8 1 2
items 3 4 4 5
fields 30 40
write 10 20 77 88
step 2 16 3
copy 77 88 4 5
generic 3 3 3
done

*/

import System;

using System;
using System.Runtime;

struct Point
{
    public Int32 X;
    public Int32 Y;
}

struct Pair
{
    public Point A;
    public Point B;
}

class Program
{
    // The pointer is moved by the size of T, which is not known here.
    static Int64 Distance<T>(Ptr<T> start, Int64 count) where T : unmanaged
    {
        Ptr<T> end = start + count;
        return end - start;
    }

    static Int64 Bytes<T>() where T : unmanaged => NativeMemory.SizeOf<T>();

    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine("sizes " + NativeMemory.SizeOf<Int8>() + " " + NativeMemory.SizeOf<Int16>() + " " + NativeMemory.SizeOf<Int32>()
            + " " + NativeMemory.SizeOf<Int64>() + " " + NativeMemory.SizeOf<Single>() + " " + NativeMemory.SizeOf<Double>()
            + " " + NativeMemory.SizeOf<Ptr>() + " " + NativeMemory.SizeOf<Ptr<Int32>>());

        Ptr<Point> points = NativeMemory.Alloc<Point>(4);

        Point a = new Point();
        a.X = 1;
        a.Y = 2;

        points.Value = a;
        Point b = points.Value;
        Console.WriteLine("struct " + NativeMemory.SizeOf<Point>() + " " + b.X + " " + b.Y);

        Point c = new Point();
        for (Int32 i = 1; i < 4; i++)
        {
            c.X = i + 2;
            c.Y = i + 3;
            points[i] = c;
        }

        Console.WriteLine("items " + points[1].X + " " + points[1].Y + " " + points[2].X + " " + points[2].Y);

        // The fields of the value that is in the memory.
        points[3].X = 30;
        points[3].Y = 40;
        Console.WriteLine("fields " + points[3].X + " " + points[3].Y);

        Ptr<Point> p = points;
        p.Value.X = 10;
        p.Value.Y = 20;
        (p + 1).Value.X = 77;
        p[1].Y = 88;
        Console.WriteLine("write " + points[0].X + " " + points[0].Y + " " + points[1].X + " " + points[1].Y);

        // The step is the size of the struct.
        Ptr<Point> q = points + 2;
        Console.WriteLine("step " + (q - points) + " " + (q.ToInt64() - points.ToInt64()) + " " + ((q + 1) - points));

        // Copy of values.
        Ptr<Point> other = NativeMemory.Alloc<Point>(2);
        NativeMemory.CopyValues(other, points + 1, 2);
        Console.WriteLine("copy " + other[0].X + " " + other[0].Y + " " + other[1].X + " " + other[1].Y);

        // The generic param.
        Console.WriteLine("generic " + Distance(points, 3) + " " + Distance(points, 3) / 1 + " " + Distance(NativeMemory.Alloc<Int64>(1), 3));
        Console.WriteLine("done");

        NativeMemory.Free(other);
        NativeMemory.Free(points);
    }
}
