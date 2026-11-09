// A generic method that is called by an interface, or by a virtual call: the method that is run
// is found by the object, and it gets the generic arguments of the call.

/* EXPECT

1
1
2
2
20
20
x
7
tag:1
tag:1
tag:2

*/

import System;

using System;
using System.Collections;

public interface IMaker
{
    Int32 Make<TKey>(TKey key);
}

public interface IMakerOf<T>
{
    Int32 Make<TKey>(TKey key, T item);
}

public class Maker : IMaker
{
    public Int32 Make<TKey>(TKey key)
    {
        return 1;
    }
}

public class MakerOf<T> : IMakerOf<T>
{
    public Int32 Make<TKey>(TKey key, T item)
    {
        return 2;
    }
}

public class Base
{
    public virtual Int32 Pick<T>(T value)
    {
        return 10;
    }
}

public class Derived : Base
{
    public override Int32 Pick<T>(T value)
    {
        return 20;
    }
}

// The generic method uses the generic argument of the call.
public interface IKeeper
{
    T Keep<T>(T value);
}

public class Keeper : IKeeper
{
    public T Keep<T>(T value)
    {
        T kept = value;
        return kept;
    }
}

public static class Tools
{
    public static T Get<T>(List<T> items)
    {
        return items[0];
    }

    // A call, and an indexer, that are the arguments of a delegate of a generic type.
    public static Void Show<T>(List<T> items, Delegate<Void, T> action)
    {
        action(Get<T>(items));
        action(items[0]);
    }

    public static Void Each<T>(T value, Delegate<Void, T> action)
    {
        action(Keep<T>(value));
    }

    private static T Keep<T>(T value)
    {
        return value;
    }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        IMaker maker = new Maker();
        Console.WriteLine(maker.Make<Int32>(5));
        Console.WriteLine(maker.Make<String>("s"));

        IMakerOf<Int32> makerOf = new MakerOf<Int32>();
        Console.WriteLine(makerOf.Make<String>("s", 1));
        Console.WriteLine(makerOf.Make<Int32>(4, 1));

        Base derived = new Derived();
        Console.WriteLine(derived.Pick<Int32>(1));
        Console.WriteLine(derived.Pick<String>("x"));

        IKeeper keeper = new Keeper();
        Console.WriteLine(keeper.Keep<String>("x"));
        Console.WriteLine(keeper.Keep<Int32>(7));

        // The arguments of a delegate that are the results of calls.
        List<String> items = new List<String>();
        items.Add("tag");
        Tools.Show<String>(items, (String s) => Console.WriteLine(s + ":1"));

        Tools.Each<String>("tag", (String s) => Console.WriteLine(s + ":2"));
    }
}
