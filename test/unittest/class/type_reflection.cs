// System.Reflection.Type: Type.GetType(obj) is the type of an object (Object has no GetType), its
// name and its full name; typeof gives the same type.

/* EXPECT

name Plain Circle
full Plain Shapes.Circle
same True False
typeof Plain Int32 True
reflection Reflection.Type
done

*/

import System;

using System;
using System.Reflection;

class Plain { }

namespace Shapes
{
    class Circle { }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Plain plain = new Plain();
        Type a = Type.GetType(plain);
        Type b = Type.GetType(new Plain());
        Type c = Type.GetType(new Shapes.Circle());

        Console.WriteLine("name " + a.Name + " " + c.Name);
        Console.WriteLine("full " + a.FullName + " " + c.FullName);
        Console.WriteLine("same " + a.Equals(b) + " " + a.Equals(c));
        Console.WriteLine("typeof " + typeof(Plain).FullName + " " + typeof(Int32).FullName
                                   + " " + a.Equals(typeof(Plain)));
        Console.WriteLine("reflection " + typeof(Type).FullName);
        Console.WriteLine("done");
    }
}
