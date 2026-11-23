// The params of an alias have the constraints of the places where they are used: the types that satisfy all of them are the
// ones that the alias takes. Where an alias is used with a type that does not, the error is in the use.

/* EXPECT

7
2 3
leaf 5
made 4
11

*/

import System;

using System;

class Box<T> where T : struct
{
    public T V;
}

class Two<A, B> where A : struct where B : struct
{
    public A First;
    public B Second;
}

interface IShape
{
    Int32 Sides();
}

class Shape : IShape
{
    public virtual Int32 Sides() { return 0; }
}

class Square : Shape
{
    public override Int32 Sides() { return 4; }
}

class Holder<X, Y> where X : Shape where Y : IShape
{
    public X Left;
    public Y Right;

    public Int32 Total() { return Left.Sides() + Right.Sides(); }
}

class Maker<T> where T : new()
{
    public static Int32 Count() { return 4; }
}

typedef SBox<T> = Box<T>;
typedef Same<T> = Two<T, T>;
typedef Both<T> = Holder<T, T>;
typedef Make<T> = Maker<T>;
typedef Sum<A, B> = Two<A, B>;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        SBox<Int32> box = new SBox<Int32>();
        box.V = 7;
        Console.WriteLine(box.V);

        Same<Int32> same = new Same<Int32>();
        same.First = 2; same.Second = 3;
        Console.WriteLine(same.First + " " + same.Second);

        Both<Square> both = new Both<Square>();
        both.Left = new Square(); both.Right = new Square();
        Console.WriteLine("leaf " + (both.Total() - 3));

        Console.WriteLine("made " + Make<Square>.Count());

        Sum<Int32, Int64> sum = new Sum<Int32, Int64>();
        sum.First = 5; sum.Second = 6;
        Console.WriteLine(sum.First + sum.Second);
    }
}
