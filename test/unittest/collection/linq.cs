// System.Linq: the methods of System.Linq.Enumerable, as extension methods of IEnumerable<T>,
// that are in the assembly System and are used by this program.
//
// A type has no default equality or order: the methods that compare the items take the
// comparer (Int32Comparer, StringComparer).

/* EXPECT

count 5 3 True True
first 1 5 5 -1
where 1 3 5
where-index 1 3 5
select n1 n2 n3 n4 n5
select-index 0 2 6 12 20
selectmany 1 1 2
take 1 2
skip 4 5
takewhile 1 2
skipwhile 3 4 5
sum 15 15 15 3
average 3 2.5
minmax 1 5 1 5
aggregate 15 -15 count5 120
concat 1 2 3 4
distinct 3 4 1
union 1 2 3 4
intersect 2 3
except 1
reverse 5 4 3 2 1
contains True False
sequenceequal True False
orderby 1 2 3 4 5
orderbydescending 5 4 3 2 1
thenby a1 a2 b1 b2
range 3 4 5 6
repeat 7 7 7
empty 0
zip 1a 2b
defaultifempty 0 9
elementat 3 0
last 5 5 4
single 12 3
tolist 3
toarray 3
todictionary 2 20
group 1:1,3,5 0:2,4
lookup 2 1,3 True False
groupby-result 1=3 0=2
chain 4 16 36
lazy made 0 1
done

*/

import System;

using System;
using System.Collections;
using System.Linq;

class Person
{
    public Person(String name, Int32 age)
    {
        Name = name;
        Age = age;
    }

    public String Name;
    public Int32 Age;
}

class Program
{
    static List<Int32> Numbers(Int32 count)
    {
        List<Int32> list = new List<Int32>();
        for (Int32 i = 1; i <= count; i++)
            list.Add(i);

        return list;
    }

    static Int32 Calls;

    static Int32 Counted(Int32 value)
    {
        Calls++;
        return value;
    }

    static String Join(IEnumerable<Int32> items)
    {
        return Join(items, " ");
    }

    static String Join(IEnumerable<Int32> items, String separator)
    {
        String text = "";
        foreach (Int32 item in items)
            text = text == ""? "" + item : text + separator + item;

        return text;
    }

    static String JoinStrings(IEnumerable<String> items)
    {
        String text = "";
        foreach (String item in items)
            text = text == ""? item : text + " " + item;

        return text;
    }

    [EntryPoint]
    public static void Main()
    {
        List<Int32> list = Numbers(5);
        Int32Comparer ints = new Int32Comparer();
        StringComparer strings = new StringComparer();

        Console.WriteLine("count " + list.Count() + " " + list.Count(v => v > 2) + " "
                                   + list.Any() + " " + list.All(v => v > 0));

        List<Int32> empty = new List<Int32>();
        Console.WriteLine("first " + list.First() + " " + list.Last() + " "
                                   + list.First(v => v > 4) + " "
                                   + (empty.FirstOrDefault(v => v > 0) - 1));

        Console.WriteLine("where " + Join(list.Where(v => v % 2 == 1)));
        Console.WriteLine("where-index " + Join(list.Where((v, i) => i % 2 == 0)));
        Console.WriteLine("select " + JoinStrings(list.Select(v => "n" + v)));
        Console.WriteLine("select-index " + Join(list.Select((v, i) => v * i)));
        Console.WriteLine("selectmany " + Join(list.Take(2).SelectMany(v => Numbers(v))));
        Console.WriteLine("take " + Join(list.Take(2)));
        Console.WriteLine("skip " + Join(list.Skip(3)));
        Console.WriteLine("takewhile " + Join(list.TakeWhile(v => v < 3)));
        Console.WriteLine("skipwhile " + Join(list.SkipWhile(v => v < 3)));

        Console.WriteLine("sum " + list.Sum() + " " + list.Sum(v => v) + " "
                                  + list.SumInt64(v => (Int64)v) + " "
                                  + list.Take(2).SumDouble(v => (Double)v));
        Console.WriteLine("average " + list.Average() + " " + list.Take(4).Average());
        Console.WriteLine("minmax " + list.Min() + " " + list.Max() + " "
                                    + list.Min(ints) + " " + list.Max(ints));
        Console.WriteLine("aggregate " + list.Aggregate((a, b) => a + b) + " "
                                       + list.Aggregate(0, (a, b) => a - b) + " "
                                       + list.Aggregate("count", (a, b) => b == 5? a + b : a)
                                       + " " + list.Aggregate(1, (a, b) => a * b));

        List<Int32> other = new List<Int32>();
        other.Add(3); other.Add(4); other.Add(3);
        Console.WriteLine("concat " + Join(list.Take(2).Concat(other.Skip(0).Take(2))));
        Console.WriteLine("distinct " + Join(other.Concat(list.Take(3)).Distinct(ints).Take(3)));
        Console.WriteLine("union " + Join(list.Take(3).Union(other, ints)));
        Console.WriteLine("intersect " + Join(list.Take(3).Intersect(other.Take(1).Concat(list.Skip(1).Take(1)), ints)));
        Console.WriteLine("except " + Join(list.Take(3).Except(list.Skip(1), ints)));
        Console.WriteLine("reverse " + Join(list.Reverse()));
        Console.WriteLine("contains " + list.Contains(3, ints) + " " + list.Contains(9, ints));
        Console.WriteLine("sequenceequal " + list.SequenceEqual(Numbers(5), ints) + " "
                                          + list.SequenceEqual(Numbers(4), ints));

        List<Int32> shuffled = new List<Int32>();
        shuffled.Add(3); shuffled.Add(1); shuffled.Add(5); shuffled.Add(2); shuffled.Add(4);
        Console.WriteLine("orderby " + Join(shuffled.OrderBy(v => v, ints)));
        Console.WriteLine("orderbydescending " + Join(shuffled.OrderByDescending(v => v, ints)));

        List<String> words = new List<String>();
        words.Add("a2"); words.Add("b2"); words.Add("a1"); words.Add("b1");
        Console.WriteLine("thenby " + JoinStrings(words.OrderByDescending(w => w.Length, ints)
                                                       .ThenBy(w => w, strings)));

        Console.WriteLine("range " + Join(Enumerable.Range(3, 4)));
        Console.WriteLine("repeat " + Join(Enumerable.Repeat(7, 3)));
        Console.WriteLine("empty " + Enumerable.Empty<Int32>().Count());

        List<String> letters = new List<String>();
        letters.Add("a"); letters.Add("b");
        Console.WriteLine("zip " + JoinStrings(list.Zip(letters, (n, c) => n + c)));

        Console.WriteLine("defaultifempty " + Join(empty.DefaultIfEmpty()) + " "
                                            + Join(empty.DefaultIfEmpty(9)));
        Console.WriteLine("elementat " + list.ElementAt(2) + " " + list.ElementAtOrDefault(9));
        Console.WriteLine("last " + list.Last(v => v < 6) + " " + list.LastOrDefault() + " "
                                 + list.LastOrDefault(v => v < 5));
        Console.WriteLine("single " + Numbers(1).Single() + 2 + " " + list.Single(v => v == 3));

        Console.WriteLine("tolist " + list.Where(v => v > 2).ToList().Count);
        Console.WriteLine("toarray " + list.Where(v => v > 2).ToArray().Length);

        List<Person> people = new List<Person>();
        people.Add(new Person("ann", 10));
        people.Add(new Person("bob", 20));
        Dictionary<String, Int32> ages = people.ToDictionary(p => p.Name, p => p.Age, strings);
        Console.WriteLine("todictionary " + ages.Count + " " + ages["bob"]);

        // The groups, in the order of their keys.
        String grouped = "";
        foreach (IGrouping<Int32, Int32> g in list.GroupBy(v => v % 2, ints))
        {
            String values = "";
            foreach (Int32 v in g)
                values = values == ""? "" + v : values + "," + v;

            grouped = grouped + (grouped == ""? "" : " ") + g.Key + ":" + values;
        }

        Console.WriteLine("group " + grouped);

        ILookup<Int32, Int32> lookup = list.ToLookup(v => v % 2, ints);
        Console.WriteLine("lookup " + lookup.Count + " " + Join(lookup[1].Take(2), ",")
                                    + " " + lookup.Contains(1) + " " + lookup.Contains(7));

        String counts = "";
        foreach (String r in list.GroupBy(v => v % 2, (key, group) => key + "=" + group.Count(), ints))
            counts = counts + (counts == ""? "" : " ") + r;

        Console.WriteLine("groupby-result " + counts);

        Console.WriteLine("chain " + Join(list.Where(v => v % 2 == 0).Select(v => v * v)
                                          .Concat(Enumerable.Range(6, 1).Select(v => v * v))));

        // Nothing is done until the items are asked.
        Calls = 0;
        IEnumerable<Int32> lazy = list.Select(v => Counted(v));
        Console.WriteLine("lazy made " + Calls + " " + lazy.Take(1).Count() * Calls);
        Console.WriteLine("done");
    }
}
