// The default order and equality of a type: Comparer<T>.Default and EqualityComparer<T>.Default
// are those of the primitive types, IComparable<T> and IEquatable<T> of a type, or Equals and
// IEquality of the objects. The collections and the methods of System.Linq use them when no
// comparer is given.

/* EXPECT

compare -1 0 1 1 1
equal True False True False
object True False True
struct True False -2
distinct 3,1,2
union 1,2,3,4
intersect 2,3
except 1
group 1:3 0:2
dictionary 2 b
order 1,2,3,5
order desc 5,3,2,1
then a1,a2,b1
minmax 1 9 a z
contains True False
sequence True False
list 1 True
keys 2 x
person 2
done

*/

import System;

using System;
using System.Collections;
using System.Linq;

struct Version : IComparable<Version>, IEquatable<Version>, IEquality
{
    public Int32 Major;

    public Int32 CompareTo(Version other) => this.Major - other.Major;

    public Boolean Equals(Version other) => this.Major == other.Major;

    public Boolean Equals(Object other) => other is Version && Equals((Version)other);

    public Int32 GetHashCode() => this.Major;
}

class Person : IEquatable<Person>, IEquality
{
    public String Name;

    public Person(String name) { this.Name = name; }

    public Boolean Equals(Person other) => this.Name == other.Name;

    public Boolean Equals(Object other) => other is Person && Equals((Person)other);

    public Int32 GetHashCode() => this.Name.GetHashCode();
}

class Plain { }

class Entry : IStringable
{
    public String Group;
    public Int32 Number;

    public Entry(String group, Int32 number)
    {
        this.Group = group;
        this.Number = number;
    }

    public String ToString() => this.Group + this.Number;
}

class Program
{
    static String Join(IEnumerable<Int32> items)
    {
        String text = "";
        foreach (Int32 item in items)
            text = text + (text == ""? "" : ",") + item;

        return text;
    }

    static String Join(IEnumerable<Entry> items)
    {
        String text = "";
        foreach (Entry item in items)
            text = text + (text == ""? "" : ",") + item.ToString();

        return text;
    }

    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine("compare " + Comparer<Int32>.Default.Compare(1, 2)
                                + " " + Comparer<String>.Default.Compare("a", "a")
                                + " " + Comparer<Int64>.Default.Compare((Int64)5, (Int64)2)
                                + " " + Comparer<Boolean>.Default.Compare(true, false)
                                + " " + Comparer<Double>.Default.Compare((Double)2.5, (Double)1.5));

        Console.WriteLine("equal " + EqualityComparer<Int32>.Default.Equals(3, 3)
                                + " " + EqualityComparer<String>.Default.Equals("a", "b")
                                + " " + EqualityComparer<Char>.Default.Equals('x', 'x')
                                + " " + EqualityComparer<UInt8>.Default.Equals((UInt8)1, (UInt8)2));

        // Objects are equal when they are the same one, unless the type says otherwise.
        Plain plain = new Plain();
        Console.WriteLine("object " + EqualityComparer<Plain>.Default.Equals(plain, plain)
                                + " " + EqualityComparer<Plain>.Default.Equals(plain, new Plain())
                                + " " + (EqualityComparer<Plain>.Default.GetHashCode(plain)
                                         == EqualityComparer<Plain>.Default.GetHashCode(plain)));

        Version v1 = new Version();
        v1.Major = 1;
        Version v3 = new Version();
        v3.Major = 3;
        Version another = new Version();
        another.Major = 1;
        Console.WriteLine("struct " + EqualityComparer<Version>.Default.Equals(v1, another)
                                 + " " + EqualityComparer<Version>.Default.Equals(v1, v3)
                                 + " " + Comparer<Version>.Default.Compare(v1, v3));

        Int32[] numbers = new Int32[] { 3, 1, 2, 3, 1 };
        Console.WriteLine("distinct " + Join(numbers.Distinct()));
        Console.WriteLine("union " + Join(new Int32[] { 1, 2, 3 }.Union(new Int32[] { 3, 4 })));
        Console.WriteLine("intersect " + Join(new Int32[] { 1, 2, 3 }.Intersect(new Int32[] { 3, 2, 5 })));
        Console.WriteLine("except " + Join(new Int32[] { 1, 2, 3 }.Except(new Int32[] { 3, 2, 5 })));

        IEnumerable<IGrouping<Int32, Int32>> groups = new Int32[] { 1, 2, 3, 4, 5 }.GroupBy(v => v % 2);
        String grouped = "";
        foreach (IGrouping<Int32, Int32> group in groups)
            grouped = grouped + (grouped == ""? "" : " ") + group.Key + ":" + group.Count();

        Console.WriteLine("group " + grouped);

        Dictionary<String, String> map = new String[] { "a", "b" }.ToDictionary(s => s, s => s);
        Console.WriteLine("dictionary " + map.Count + " " + map["b"]);

        Console.WriteLine("order " + Join(new Int32[] { 3, 1, 5, 2 }.OrderBy(v => v)));
        Console.WriteLine("order desc " + Join(new Int32[] { 3, 1, 5, 2 }.OrderByDescending(v => v)));

        Entry[] entries = new Entry[] { new Entry("b", 1), new Entry("a", 2), new Entry("a", 1) };
        Console.WriteLine("then " + Join(entries.OrderBy(e => e.Group).ThenBy(e => e.Number)));

        Console.WriteLine("minmax " + new Int32[] { 4, 1, 9 }.Min() + " " + new Int32[] { 4, 1, 9 }.Max()
                                   + " " + new String[] { "b", "a", "c" }.Min()
                                   + " " + new String[] { "b", "z", "c" }.Max());

        Console.WriteLine("contains " + numbers.Contains(2) + " " + numbers.Contains(9));
        Console.WriteLine("sequence " + new Int32[] { 1, 2 }.SequenceEqual(new Int32[] { 1, 2 })
                                     + " " + new Int32[] { 1, 2 }.SequenceEqual(new Int32[] { 2, 1 }));

        // The collections.
        List<String> list = new List<String>();
        list.Add("a");
        list.Add("b");
        Console.WriteLine("list " + list.IndexOf("b") + " " + list.Contains("a"));

        Dictionary<String, Int32> keys = new Dictionary<String, Int32>();
        keys.Add("x", 1);
        keys.Add("y", 2);
        Console.WriteLine("keys " + keys.Count + " x");

        Person[] people = new Person[] { new Person("ann"), new Person("bob"), new Person("ann") };
        Console.WriteLine("person " + people.Distinct().Count());

        Console.WriteLine("done");
    }
}
