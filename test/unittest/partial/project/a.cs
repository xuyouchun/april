import System;

using System;
using Shapes;

namespace Shapes
{
    public partial class Circle
    {
        private int _r;

        public Circle(int r) { _r = r; _id = 7; }

        public int Radius { get { return _r; } }
    }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Circle c = new Circle(3);
        Console.WriteLine(c.Area());
        Console.WriteLine(c.Describe());
        int n = Circle.Count;
        Console.WriteLine(n);
        Console.WriteLine("done");
    }
}
