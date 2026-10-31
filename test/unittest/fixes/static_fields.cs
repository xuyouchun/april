// Static fields: shared storage, initial values and static constructors.

/* EXPECT

42
hello
7
13
2
6
3

*/

import System;

using System;

class Counter
{
    public static int Value = 42;
    public static String Name = "hello";
    public static int Total;
    public int Id;

    static Counter() { Total = 7; }

    public Counter() { Count = Count + 1; Id = Count; }

    public static int Count;

    public static void Add(int n) { Total = Total + n; }
}

class Plain
{
    public static int X = 5;
    public static int Twice() { return X * 2; }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine(Counter.Value);
        Console.WriteLine(Counter.Name);
        Console.WriteLine(Counter.Total);
        Counter.Add(6);
        Console.WriteLine(Counter.Total);

        Counter a = new Counter();
        Counter b = new Counter();
        Console.WriteLine(Counter.Count);

        Plain.X = Plain.X + 1;
        Console.WriteLine(Plain.X);
        Console.WriteLine(Plain.Twice() / 4);
    }
}
