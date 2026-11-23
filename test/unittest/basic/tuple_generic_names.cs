// The names of the items of a tuple are kept in the generic arguments: List<(Int32 A, String B)>, the element of an array, a
// var that is given a variable, the variable of a foreach, a typedef of them.

/* EXPECT

Wangbo2
1
two
one1
1one
2two
9
a3
5
b
3
x
7
2
sum 6
t

*/

import System;

using System;
using System.Collections;

typedef Rows = List<(Int32 A, String B)>;
typedef Box<T> = List<(T Key, String Text)>;

class Program
{
    static List<(Int32 A, String B)> Make()
    {
        List<(Int32 A, String B)> list = new List<(Int32 A, String B)>();
        list.Add((1, "one"));
        return list;
    }

    [EntryPoint]
    public static void Main()
    {
        // A var that is given a variable has its names.
        var tuple = (Name: "Wangbo", Value: 2, 3);
        var tuple2 = tuple;
        Console.WriteLine(tuple2.Name + tuple2.Value);

        List<(Int32 A, String B)> list = new List<(Int32 A, String B)>();
        list.Add((1, "one"));
        list.Add((2, "two"));

        Console.WriteLine(list[0].A);
        Console.WriteLine(list[1].B);

        var first = list[0];
        Console.WriteLine(first.B + first.A);

        foreach (var item in list)
            Console.WriteLine(item.A + item.B);

        // The value of a dictionary.
        Dictionary<String, (Int32 Lo, Int32 Hi)> map = new Dictionary<String, (Int32 Lo, Int32 Hi)>();
        map["a"] = (3, 9);
        Console.WriteLine(map["a"].Hi);

        foreach (var pair in map)
            Console.WriteLine(pair.Key + pair.Value.Lo);

        // An array.
        (Int32 Code, String Text)[] codes = new (Int32 Code, String Text)[] { (5, "five") };
        Console.WriteLine(codes[0].Code);

        foreach (var code in codes)
            Console.WriteLine(code.Text.Substring(0, 0) + "b");

        // A list in a list.
        List<List<(Int32 A, String B)>> nested = new List<List<(Int32 A, String B)>>();
        nested.Add(list);
        Console.WriteLine(nested[0][0].A + 2);

        // A call, a typedef, a typedef that has a param.
        Console.WriteLine(Make()[0].B.Substring(0, 0) + "x");

        Rows rows = Make();
        rows.Add((7, "seven"));
        Console.WriteLine(rows[1].A);

        Box<Int32> box = new Box<Int32>();
        box.Add((2, "two"));
        Console.WriteLine(box[0].Key);

        Int32 sum = 0;
        foreach (var r in rows)
            sum += r.A;
        Console.WriteLine("sum " + (sum - 2));

        // The items of an initializer are made for the type of the elements: a reference, or a struct.
        ITuple<Int32, String>[] refs = new ITuple<Int32, String>[] { (3, "t") };
        Console.WriteLine(refs[0].Item2);
    }
}
