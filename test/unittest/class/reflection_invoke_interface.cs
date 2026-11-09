// Method.Invoke of the methods of an interface (they are called on an object of a class that
// implements the interface: the function of the object is the one that is called, the arguments
// and the result are checked like those of any method), of a generic interface (IBox<Int32>), and of
// the methods of a number (Int32.ToString: the target is the box of the number).

/* EXPECT

shape 2
area 16
describe side 4
box 4
twice 8
number 5
compare 1
interface compare -1
wrong target caught
null target caught
done

*/

import System;

using System;
using System.Reflection;

interface IShape
{
    Int32 Area();
    String Describe(String prefix);
}

interface IBox<T>
{
    T Get();
    T Twice(T extra);
}

class Square : IShape, IBox<Int32>
{
    public Int32 Side;

    public Int32 Area() { return Side * Side; }
    public String Describe(String prefix) { return prefix + Side; }
    public Int32 Get() { return Side; }
    public Int32 Twice(Int32 extra) { return Side * 2 + extra - extra; }
}

class Other { }

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Square s = new Square();
        s.Side = 4;

        Type shape = typeof(IShape);
        Console.WriteLine("shape " + shape.GetMethods().Length);
        Console.WriteLine("area " + shape.GetMethods("Area")[0].Invoke(s, new Object[0]));
        Console.WriteLine("describe " + shape.GetMethods("Describe")[0].Invoke(s, new Object[] { "side " }));

        Type box = typeof(IBox<Int32>);
        Console.WriteLine("box " + box.GetMethods("Get")[0].Invoke(s, new Object[0]));
        Console.WriteLine("twice " + box.GetMethods("Twice")[0].Invoke(s, new Object[] { 0 }));

        Type number = typeof(Int32);
        Object five = 5;
        Console.WriteLine("number " + number.GetMethods("ToString")[0].Invoke(five, new Object[0]));

        Method compare = number.GetMethods("CompareTo")[0];
        Console.WriteLine("compare " + compare.Invoke(five, new Object[] { 3 }));

        // The method of an interface that the number implements.
        Method icompare = typeof(IComparable<Int32>).GetMethods("CompareTo")[0];
        Console.WriteLine("interface compare " + icompare.Invoke(five, new Object[] { 9 }));

        try
        {
            shape.GetMethods("Area")[0].Invoke(new Other(), new Object[0]);
        }
        catch (InvalidCastException e)
        {
            Console.WriteLine("wrong target caught");
        }

        try
        {
            shape.GetMethods("Area")[0].Invoke(null, new Object[0]);
        }
        catch (NullReferenceException e)
        {
            Console.WriteLine("null target caught");
        }

        Console.WriteLine("done");
    }
}
