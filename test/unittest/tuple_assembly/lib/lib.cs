// A library that has tuples written with names: a program that uses it reads the items by the names.

import System;

using System;
using System.Collections;

namespace Geo
{
    public typedef Point = (Int32 X, Int32 Y);
    public typedef Pair<T> = (T First, T Second);
    public typedef Table = List<(Int32 Row, Int32 Column)>;

    public class Shape
    {
        public (Int32 Width, Int32 Height) Size
        {
            get { return (3, 4); }
        }

        public (String Name, Int32 Sides) Info
        {
            get { return ("square", 4); }
        }

        public static (Int32 Id, String Label) Make(Int32 id) { return (id, "item"); }

        // The names are in the generic arguments too.
        public List<(Int32 Id, String Label)> Rows
        {
            get
            {
                List<(Int32 Id, String Label)> rows = new List<(Int32 Id, String Label)>();
                rows.Add((1, "one"));
                rows.Add((2, "two"));
                return rows;
            }
        }

        public static Dictionary<String, (Int32 Lo, Int32 Hi)> Ranges()
        {
            Dictionary<String, (Int32 Lo, Int32 Hi)> map = new Dictionary<String, (Int32 Lo, Int32 Hi)>();
            map["a"] = (3, 9);
            return map;
        }

        public static (Int32 Code, String Text)[] Codes() { return new (Int32 Code, String Text)[] { (5, "five") }; }

        public static Int32 Area((Int32 Width, Int32 Height) size) { return size.Width * size.Height; }
    }
}
