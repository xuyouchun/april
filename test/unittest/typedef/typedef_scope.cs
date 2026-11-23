// A typedef in a namespace is a member of it: internal if it has no access (the assembly sees it), public (the assemblies
// that use it see it too). A typedef that is a member of a type has the access that it is given (private if there is none), a
// nested type is reached through the type: Type.Name.

/* EXPECT

file 1
member 2
nested 3
call 5
shared 10
1
plain 8

*/

import System;

using System;
using System.Collections;

typedef Counter = Int32;

namespace Shop
{
    internal typedef Price = Int32;
    typedef Plain = Int32;
    public typedef Handler = Delegate<Int32, Int32>;

    public class Till
    {
        public typedef Items = List<Price>;
        private typedef Hidden = List<String>;

        public static Int32 Total(Items items)
        {
            Int32 t = 0;
            for (Int32 i = 0; i < items.Count; i++)
                t += items[i];
            return t;
        }

        public static Int32 Call(Handler h) { return h(4); }

        public static Int32 Count()
        {
            Hidden h = new Hidden();
            h.Add("x");
            return h.Count;
        }
    }
}

namespace Other
{
    class Use
    {
        public static Int32 Run()
        {
            Shop.Price p = 10;
            return p;
        }
    }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Counter c = 1;
        Console.WriteLine("file " + c);

        Shop.Price p = 2;
        Console.WriteLine("member " + p);

        Shop.Till.Items items = new Shop.Till.Items();
        items.Add(1);
        items.Add(2);
        Console.WriteLine("nested " + Shop.Till.Total(items));

        Shop.Handler h = x => x + 1;
        Console.WriteLine("call " + Shop.Till.Call(h));

        Console.WriteLine("shared " + Other.Use.Run());
        Console.WriteLine(Shop.Till.Count());

        Shop.Plain plain = 8;
        Console.WriteLine("plain " + plain);
    }
}
