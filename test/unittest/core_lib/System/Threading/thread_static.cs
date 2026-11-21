// [ThreadStatic]: each thread has its own copy of the field, the others are shared; the copy of a thread starts as the default
// of the type, the initial value of the field is for the thread that runs the static constructor.

/* EXPECT

main 5 shared 3 initial 7
seen 11 21 31
objects 3 True

*/

import System;

using System;
using System.Threading;

class Box
{
    [ThreadStatic] public static Int32 Value;

    [ThreadStatic] public static Int32 WithInitial = 7;

    public static Int32 Shared;

    [ThreadStatic] private static Object? __object;

    public static Object Own()
    {
        if (__object == null)
            __object = new Object();

        return __object;
    }
}

class Program
{
    static Int32[] __seen = new Int32[3];
    static Object[] __objects = new Object[3];
    static Int32 __initial;

    static Void Work(Object o)
    {
        Int32 i = (Int32)o;

        Box.Value = Box.Value + 10 * (i + 1);
        Thread.Yield();
        Box.Value = Box.Value + 1;

        Interlocked.Add(ref Box.Shared, 1);

        __seen[i] = Box.Value;
        __objects[i] = Box.Own();
        __initial = __initial + Box.WithInitial;
    }

    [EntryPoint]
    public static void Main()
    {
        Box.Value = 5;

        Delegate<Void, Object> work = Work;
        Thread[] threads = new Thread[3];
        for (Int32 i = 0; i < 3; i++)
        {
            threads[i] = new Thread(work);
            threads[i].Start(i);
        }

        for (Int32 i = 0; i < 3; i++)
            threads[i].Join();

        Console.WriteLine("main " + Box.Value + " shared " + Box.Shared + " initial " + Box.WithInitial);
        Console.WriteLine("seen " + __seen[0] + " " + __seen[1] + " " + __seen[2]);

        Int32 distinct = 0;
        for (Int32 i = 0; i < 3; i++)
        {
            Boolean fresh = !Object.ReferenceEquals(__objects[i], Box.Own());
            for (Int32 j = 0; j < i; j++)
                fresh = fresh && !Object.ReferenceEquals(__objects[i], __objects[j]);

            if (fresh)
                distinct++;
        }

        Console.WriteLine("objects " + distinct + " " + Object.ReferenceEquals(Box.Own(), Box.Own()));
    }
}
