// Span<T> and ReadOnlySpan<T>: windows on the items of an array.

/* EXPECT

basic 5 False 20 40 0 True
slice 3 30 40 2 40 0 0
share 99 77 99 99
conversion 6 6 0
copy 0 4 5 6 0 False True
forward 1 1 2 3 4
backward 2 3 4 5 5
fill 7 7 7 4 5
foreach 276 abc
tostring 2 20 True
struct 10 20 | 10 20 | 5 7
readonly 24 20 caught caught caught caught
generic 6 xy 3
field 3 3 True
array 2 1 2
nullarray 0 True caught

*/

import System;

using System;

struct Point
{
    public Point(Int32 x, Int32 y)
    {
        X = x;
        Y = y;
    }

    public Int32 X;

    public Int32 Y;
}

class Holder
{
    public Span<Int32> Window;

    public ReadOnlySpan<Int32> View;
}

class Program
{
    static Int32 Sum(ReadOnlySpan<Int32> items)
    {
        Int32 total = 0;

        for (Int32 k = 0; k < items.Length; k++)
            total += items[k];

        return total;
    }

    static T First<T>(Span<T> items) => items[0];

    static Int32 Count<T>(ReadOnlySpan<T> items) => items.Length;

    static Void Zero(Span<Int32> items)
    {
        for (Int32 k = 0; k < items.Length; k++)
            items[k] = 0;
    }

    static String Join(Int32[] items)
    {
        String text = "";

        for (Int32 k = 0; k < items.Length; k++)
            text += (k > 0 ? " " : "") + items[k];

        return text;
    }

    [EntryPoint]
    public static void Main()
    {
        Int32[] items = new Int32[] { 10, 20, 30, 40, 50 };
        Span<Int32> all = new Span<Int32>(items);
        Console.WriteLine("basic " + all.Length + " " + all.IsEmpty + " " + all[1] + " " + all[3]
            + " " + Span<Int32>.Empty.Length + " " + Span<Int32>.Empty.IsEmpty);

        Span<Int32> mid = all.Slice(2);
        Span<Int32> two = all.Slice(1, 2);
        Span<Int32> inner = mid.Slice(1, 1);
        Console.WriteLine("slice " + mid.Length + " " + mid[0] + " " + mid[1] + " " + two.Length + " " + inner[0]
            + " " + all.Slice(5).Length + " " + all.Slice(0, 0).Length);

        // A change of a span is a change of the array, and of the other spans on it.
        mid[0] = 99;
        two[0] = 77;
        Console.WriteLine("share " + items[2] + " " + items[1] + " " + all[2] + " " + two[1]);

        // An array is a span, a span is a read-only span.
        Int32[] numbers = new Int32[] { 1, 2, 3 };
        Span<Int32> fromArray = numbers;
        Console.WriteLine("conversion " + Sum(numbers) + " " + Sum(fromArray) + " " + Sum(fromArray.Slice(0, 0)));

        Int32[] source = new Int32[] { 4, 5, 6 };
        Int32[] target = new Int32[] { 0, 0, 0, 0, 0 };
        new Span<Int32>(source).CopyTo(new Span<Int32>(target, 1, 3));
        Console.WriteLine("copy " + target[0] + " " + target[1] + " " + target[2] + " " + target[3] + " " + target[4]
            + " " + new Span<Int32>(source).TryCopyTo(new Span<Int32>(target, 3, 2))
            + " " + new Span<Int32>(source).TryCopyTo(new Span<Int32>(target)));

        // The ranges of one array can overlap, in both directions.
        Int32[] a = new Int32[] { 1, 2, 3, 4, 5 };
        new Span<Int32>(a, 0, 4).CopyTo(new Span<Int32>(a, 1, 4));
        Console.WriteLine("forward " + Join(a));

        Int32[] b = new Int32[] { 1, 2, 3, 4, 5 };
        new Span<Int32>(b, 1, 4).CopyTo(new Span<Int32>(b, 0, 4));
        Console.WriteLine("backward " + Join(b));

        Int32[] f = new Int32[] { 1, 2, 3, 4, 5 };
        new Span<Int32>(f, 0, 3).Fill(7);
        Console.WriteLine("fill " + Join(f));

        Int32 total = 0;
        foreach (Int32 v in all)
            total += v;

        String[] words = new String[] { "a", "b", "c" };
        String joined = "";
        foreach (String w in new ReadOnlySpan<String>(words))
            joined += w;

        Console.WriteLine("foreach " + total + " " + joined);

        // ToArray is a copy.
        Int32[] copy = all.Slice(1, 2).ToArray();
        copy[0] = -1;
        Console.WriteLine("tostring " + copy.Length + " " + items[3] / 2 + " " + (items[1] != -1));

        // The items that are values of a struct.
        Point[] points = new Point[3];
        points[0] = new Point(1, 2);
        points[1] = new Point(10, 20);
        points[2] = new Point(5, 7);
        Span<Point> ps = new Span<Point>(points);
        ps[0] = ps[1];
        Console.WriteLine("struct " + points[0].X + " " + points[0].Y + " | " + ps[1].X + " " + ps[1].Y
            + " | " + ps.Slice(2)[0].X + " " + ps.Slice(2)[0].Y);

        // Read-only: a window on the array that the others can change.
        Int32[] shared = new Int32[] { 1, 2, 3 };
        ReadOnlySpan<Int32> view = shared;
        shared[1] = 20;
        String caughtSlice = "no", caughtIndex = "no", caughtCtor = "no", caughtCopy = "no";
        try { ReadOnlySpan<Int32> t = view.Slice(2, 2); } catch (OutOfRangeException) { caughtSlice = "caught"; }
        try { Int32 x = view[3]; } catch (OutOfRangeException) { caughtIndex = "caught"; }
        try { ReadOnlySpan<Int32> t = new ReadOnlySpan<Int32>(shared, 1, 3); } catch (OutOfRangeException) { caughtCtor = "caught"; }
        try { view.CopyTo(new Span<Int32>(new Int32[2])); } catch (OutOfRangeException) { caughtCopy = "caught"; }
        Console.WriteLine("readonly " + Sum(view) + " " + view[1] + " " + caughtSlice + " " + caughtIndex + " " + caughtCtor + " " + caughtCopy);

        Console.WriteLine("generic " + First(new Span<Int32>(new Int32[] { 6, 7 })) + " "
            + First(new Span<String>(new String[] { "xy", "z" })) + " " + Count<String>(new String[] { "x", "y", "z" }));

        // A span is kept in a field of an object.
        Holder h = new Holder();
        h.Window = new Span<Int32>(items, 1, 3);
        h.View = h.Window;
        Zero(h.Window.Slice(0, 1));
        Console.WriteLine("field " + h.Window.Length + " " + h.View.Length + " " + (items[1] == 0));

        Span<Int32>[] spans = new Span<Int32>[2];
        spans[0] = new Span<Int32>(numbers, 0, 1);
        spans[1] = new Span<Int32>(numbers, 1, 2);
        Console.WriteLine("array " + spans.Length + " " + spans[0].Length + " " + spans[1].Length);

        Span<Int32> none = new Span<Int32>(default(Int32[]));
        String caughtNull = "no";
        try { Span<Int32> t = new Span<Int32>(default(Int32[]), 0, 2); } catch (OutOfRangeException) { caughtNull = "caught"; }
        Console.WriteLine("nullarray " + none.Length + " " + none.IsEmpty + " " + caughtNull);
    }
}
