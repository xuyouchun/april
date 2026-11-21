// A delegate that a field of an object holds (an instance field, a static one, a field of a struct) is called through the
// object: obj.Field(args).

/* EXPECT

hit
sum 7
static 3
struct 12

*/

import System;

using System;

class Holder
{
    public Delegate<Void> Hit;
    public Delegate<Int32, Int32, Int32> Add;
    public static Delegate<Int32, Int32> Triple;
}

struct Pair
{
    public Delegate<Int32, Int32> Square;
}

class Program
{
    static Void Hit() { Console.WriteLine("hit"); }

    static Int32 Add(Int32 a, Int32 b) { return a + b; }

    static Int32 Triple(Int32 a) { return a * 3; }

    static Int32 Square(Int32 a) { return a * a; }

    [EntryPoint]
    public static void Main()
    {
        Holder h = new Holder();
        h.Hit = Hit;
        h.Add = Add;
        Holder.Triple = Triple;

        h.Hit();
        Console.WriteLine("sum " + h.Add(3, 4));
        Console.WriteLine("static " + Holder.Triple(1));

        Pair p = new Pair();
        p.Square = Square;
        Console.WriteLine("struct " + (p.Square(3) + 3));
    }
}
