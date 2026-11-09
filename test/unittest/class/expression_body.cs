// A method, a property, an indexer and an accessor have an expression as the body: => x; is
// { return x; }, and for a method that returns nothing (or a setter) the expression is run.

/* EXPECT

value 10
property 20
accessor 30
sum 7
static 5
set 9 9
indexer 4
void run
operator 12
ctor 3
generic abc
done

*/

import System;

using System;

class Box
{
    private Int32 __value = 0;

    public Int32 GetValue() => 10;
    public Int32 Property => 20;
    public Int32 Accessor { get => 30; }

    public Int32 Value
    {
        get => this.__value;
        set => this.__value = value;
    }

    public Int32 this[Int32 index] => index * 2;

    public static Int32 Sum(Int32 a, Int32 b) => a + b;
    public static Int32 Five => 5;

    public void Run() => Console.WriteLine("void run");

    public static Box operator +(Box a, Box b) => new Box(a.__value + b.__value);

    public Box() => Console.Write("");

    public Box(Int32 value) : this() => this.__value = value;
}

class Holder<T>
{
    private T __item;

    public Holder(T item) { this.__item = item; }

    public T Item => this.__item;
    public T Get() => this.__item;
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Box box = new Box();
        Console.WriteLine("value " + box.GetValue());
        Console.WriteLine("property " + box.Property);
        Console.WriteLine("accessor " + box.Accessor);
        Console.WriteLine("sum " + Box.Sum(3, 4));
        Console.WriteLine("static " + Box.Five);

        box.Value = 9;
        Console.WriteLine("set " + box.Value + " " + box.Value);
        Console.WriteLine("indexer " + box[2]);
        box.Run();

        Box sum = new Box(5) + new Box(7);
        Console.WriteLine("operator " + sum.Value);
        Console.WriteLine("ctor " + new Box(3).Value);

        Holder<String> holder = new Holder<String>("abc");
        Console.WriteLine("generic " + holder.Item);
        Console.WriteLine("done");
    }
}
