// The values of a dictionary can be of any type: objects, strings, numbers, objects of generic
// types (lists, other dictionaries), the generic type of the values is a param of a method too.

/* EXPECT

object 2 s
list 2 7
nested 1 5
person 2 ann
generic 3 2
done

*/

import System;

using System;
using System.Collections;

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

class Group<TKey, TElement>
{
    public Group(TKey key)
    {
        Key = key;
        Items = new List<TElement>();
    }

    public TKey Key;
    public List<TElement> Items;
}

class Program
{
    // A dictionary of the groups of the items: the type of the values has the generic params.
    static Int32 Count<TKey, T>(List<T> items, Delegate<TKey, T> keyOf, IEqualityComparer<TKey> comparer)
    {
        Dictionary<TKey, Group<TKey, T>> groups = new Dictionary<TKey, Group<TKey, T>>(comparer);

        foreach (T item in items)
        {
            TKey key = keyOf(item);

            Group<TKey, T> group;
            if (!groups.TryGetValue(key, out group))
            {
                group = new Group<TKey, T>(key);
                groups.Add(key, group);
            }

            group.Items.Add(item);
        }

        return groups.Count;
    }

    [EntryPoint]
    public static void Main()
    {
        Int32Comparer ints = new Int32Comparer();
        StringComparer strings = new StringComparer();

        // Object values.
        Dictionary<Int32, Object> objects = new Dictionary<Int32, Object>(ints);
        objects.Add(1, "s");
        objects.Add(2, new Person("bob", 20));
        Object first;
        objects.TryGetValue(1, out first);
        Console.WriteLine("object " + objects.Count + " " + (String)first);

        // The values are lists.
        Dictionary<Int32, List<Int32>> lists = new Dictionary<Int32, List<Int32>>(ints);
        lists.Add(1, new List<Int32>());
        lists.Add(2, new List<Int32>());
        List<Int32> second;
        lists.TryGetValue(2, out second);
        second.Add(7);
        Console.WriteLine("list " + lists.Count + " " + second[0]);

        // The values are dictionaries.
        Dictionary<String, Dictionary<Int32, Int32>> nested =
                    new Dictionary<String, Dictionary<Int32, Int32>>(strings);
        Dictionary<Int32, Int32> inner = new Dictionary<Int32, Int32>(ints);
        inner.Add(5, 5);
        nested.Add("a", inner);
        Dictionary<Int32, Int32> found;
        nested.TryGetValue("a", out found);
        Console.WriteLine("nested " + nested.Count + " " + found[5]);

        // The values are objects of a class that has both references and numbers.
        Dictionary<String, Person> people = new Dictionary<String, Person>(strings);
        people.Add("ann", new Person("ann", 10));
        people.Add("bob", new Person("bob", 20));
        Person ann;
        people.TryGetValue("ann", out ann);
        Console.WriteLine("person " + people.Count + " " + ann.Name);

        // The type of the values is generic, in a generic method.
        List<Int32> numbers = new List<Int32>();
        numbers.Add(1); numbers.Add(2); numbers.Add(3);
        Console.WriteLine("generic " + numbers.Count + " " + Count<Int32, Int32>(numbers, v => v % 2, ints));

        Console.WriteLine("done");
    }
}
