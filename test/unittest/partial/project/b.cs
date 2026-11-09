import System;

using System;

namespace Shapes
{
    public partial class Circle
    {
        private int _id;

        public static int Count = 2;

        public int Area() { return 3 * _r * _r; }

        public String Describe()
        {
            Console.WriteLine("id " + _id);
            return "circle " + Radius;
        }
    }
}
