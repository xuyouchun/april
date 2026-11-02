// A method of the object as a delegate with no object in front: Delegate<int, int> g = Plain;

/* EXPECT

1005
1
105
105

*/

import System;

using System;
class Program
{
    int bias = 100;
    int Plain(int a)
    {
        return a + bias;
    }

    static int Sta(int a)
    {
        return a + 1000;
    }
    int A1(int x)
    {
        Delegate<int, int> g = this.Plain;
        return g(x);
    }
    int A2(int x)
    {
        Delegate<int, int> g = Sta;
        return g(x);
    }
    int A3(int x)
    {
        Program q = this;
        Delegate<int, int> g = q.Plain;
        return g(x);
    }
    int A4(int x)
    {
        Delegate<int, int> g = Plain;
        return 1;
    }

    [EntryPoint]
    public static void Main()
    {
        Program p = new Program();
        Console.WriteLine(p.A2(5));
        Console.WriteLine(p.A4(5));
        Console.WriteLine(p.A3(5));
        Console.WriteLine(p.A1(5));
    }
}
