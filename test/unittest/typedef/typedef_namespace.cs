// using Name = A.B; is the alias of a namespace (the types of the namespace are then Name.Type); the alias of a type is a
// typedef. A typedef in a namespace is a member of it (internal if it has no access), and the names of its type are those
// of the namespace where it is.

/* EXPECT

2
0
inner 9
wide 4

*/

import System;

using System;
using C = System.Collections;
typedef Num = System.Int32;

namespace Left
{
    class Cell
    {
        public static Int32 Nine() { return 9; }
    }

    typedef Near = Cell;

    class Use
    {
        public static Int32 Run() { return Near.Nine(); }
    }
}

namespace Right
{
    class Use
    {
        public static Num Wide()
        {
            Left.Near n = null;
            return 4;
        }
    }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        C.List<Int32> list = new C.List<Int32>();
        list.Add(1);
        list.Add(2);
        Console.WriteLine(list.Count);

        C.List<String> empty = new C.List<String>();
        Console.WriteLine(empty.Count);

        Console.WriteLine("inner " + Left.Use.Run());
        Console.WriteLine("wide " + Right.Use.Wide());
    }
}
