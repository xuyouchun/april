// A generic class that overrides the virtual methods of a generic base class (three levels, an
// abstract root), a class that overrides one method and declares another virtual after it, the
// fields of a generic base that are used by their names, and the value of a field of a generic
// param that is made a string (this.Value).

/* EXPECT

derived
7
7
Leaf.Name
text
derived
Base.Name
base
Order.Extra
Order.Name
Derived 5
Plain 3
Inherited 4
done

*/

import System;

using System;

abstract class Root<T>
{
    public abstract String Kind();
    public virtual T Echo(T x) { return x; }
}

class Base<T> : Root<T>
{
    public T Value;

    public override String Kind() { return "base"; }
    public virtual String Name() { return "Base.Name"; }
    public virtual T Get() { return this.Value; }
    public virtual Void Set(T x) { this.Value = x; }
    public virtual String Describe() { return "Base " + this.Value; }
}

class Derived<T> : Base<T>
{
    public override String Kind() { return "derived"; }
    public override T Get() { return this.Value; }
    public override Void Set(T x) { this.Value = x; }
    public override T Echo(T x) { return this.Value; }
    public override String Describe() { return "Derived " + this.Value; }
    public T Inherited() { return Value; }
}

class Leaf<T> : Derived<T>
{
    public override String Name() { return "Leaf.Name"; }
}

// Not generic: the override is before the virtual method that is new.
class Order
{
    public virtual String Kind() { return "Order.Kind"; }
}

class OrderDerived : Order
{
    public override String Kind() { return "base"; }
    public virtual String Extra() { return "Order.Extra"; }
    public virtual String Name() { return "Order.Name"; }
}

class Plain<T>
{
    public T Value;
    public String Show() { return "Plain " + this.Value; }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Root<Int32> r = new Leaf<Int32>();
        Console.WriteLine(r.Kind());

        Base<Int32> b = (Base<Int32>)r;
        b.Set(7);
        Console.WriteLine(b.Get());
        Console.WriteLine(r.Echo(1));
        Console.WriteLine(b.Name());

        Derived<String> d = new Derived<String>();
        d.Set("text");
        Console.WriteLine(d.Get());
        Console.WriteLine(d.Kind());
        Console.WriteLine(d.Name());

        OrderDerived o = new OrderDerived();
        Console.WriteLine(o.Kind());
        Console.WriteLine(o.Extra());
        Console.WriteLine(o.Name());

        Base<Int32> e = new Derived<Int32>();
        e.Set(5);
        Console.WriteLine(e.Describe());

        Plain<Int32> p = new Plain<Int32>();
        p.Value = 3;
        Console.WriteLine(p.Show());

        Derived<Int32> i = new Derived<Int32>();
        i.Value = 4;
        Console.WriteLine("Inherited " + i.Inherited());

        Console.WriteLine("done");
    }
}
