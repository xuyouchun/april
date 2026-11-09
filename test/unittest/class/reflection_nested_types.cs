// Type.GetNestedTypes(): the types that a type declares, in the order that they are declared in, and
// the ones that the compiler makes (the state of an iterator, the environment of a lambda) after
// them. The hash of the names that the compiler makes is not shown: it depends on the file.

/* EXPECT

4
Outer.First 11
Outer.Second 12
Outer.__ap_iterator_0 28
Outer.__ap_env_0 23
0
Outer.Second.Deep
0
done

*/

import System;

using System;
using System.Collections;
using System.Reflection;

class Outer
{
    public class First { }

    public class Second
    {
        public class Deep { }
    }

    public IEnumerable<Int32> Numbers()
    {
        yield return 1;
    }

    public Int32 Apply(Int32 x)
    {
        Delegate<Int32, Int32> f = a => a + x;
        return f(1);
    }
}

class Plain { }

class Program
{
    // The name without the hash of what the compiler makes (_ and 6 digits).
    static String Show(String name)
    {
        for (Int32 k = 0; k + 12 <= name.Length; k++)
        {
            if (name.Substring(k, 5) == "__ap_")
                return name.Substring(0, name.Length - 7);
        }

        return name;
    }

    [EntryPoint]
    public static void Main()
    {
        Object[] keep = new Object[] { new Outer(), new Plain(), new Outer.First(), new Outer.Second() };

        Array<Type> nested = typeof(Outer).GetNestedTypes();
        Console.WriteLine(nested.Length);
        for (Int32 k = 0; k < nested.Length; k++)
            Console.WriteLine(Show(nested[k].Name) + " " + nested[k].FullName.Length);

        Console.WriteLine(typeof(Plain).GetNestedTypes().Length);
        Console.WriteLine(typeof(Outer.Second).GetNestedTypes()[0].Name);
        Console.WriteLine(typeof(Outer.Second).GetNestedTypes()[0].GetNestedTypes().Length);
        Console.WriteLine("done");
    }
}
