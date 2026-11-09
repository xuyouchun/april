// An array is a collection: the arrays are derived from the abstract class Array<T>, that
// implements IReadOnlyList<T>. An array is used where a collection is wanted, and by the methods
// of System.Linq.

/* EXPECT

enumerable 6
list 3 1 2
collection 3
cast True True False
generic 3 abc 2
linq 3 2 3 1 2
select n1n2n3
orderby 1 2 3
tolist 3
objects 2 bob
base type 3 7
empty 0 False
array of arrays 2 3
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
    static Int32 Sum(IEnumerable<Int32> items)
    {
        Int32 sum = 0;
        foreach (Int32 item in items)
            sum += item;

        return sum;
    }

    // A generic method that takes the collections, an array too.
    static Int32 CountOf<T>(IEnumerable<T> items)
    {
        Int32 count = 0;
        foreach (T item in items)
            count++;

        return count;
    }

    static String Join(IEnumerable<String> items)
    {
        String text = "";
        foreach (String item in items)
            text = text + item;

        return text;
    }

    static String Join(IEnumerable<Int32> items)
    {
        String text = "";
        foreach (Int32 item in items)
            text = text == ""? "" + item : text + " " + item;

        return text;
    }

    [EntryPoint]
    public static void Main()
    {
        Int32[] numbers = new Int32[] { 3, 1, 2 };
        Int32Comparer ints = new Int32Comparer();

        // The array is an IEnumerable<T>.
        IEnumerable<Int32> enumerable = numbers;
        Int32 sum = 0;
        foreach (Int32 item in enumerable)
            sum += item;

        Console.WriteLine("enumerable " + sum);

        // It is an IReadOnlyList<T>, and an IReadOnlyCollection<T>.
        IReadOnlyList<Int32> list = numbers;
        Console.WriteLine("list " + list.Count + " " + list[1] + " " + list[2]);

        IReadOnlyCollection<Int32> collection = numbers;
        Console.WriteLine("collection " + collection.Count);

        // The casts.
        Object o = numbers;
        Console.WriteLine("cast " + (o is IEnumerable<Int32>) + " " + (o is IReadOnlyList<Int32>)
                                  + " " + (o is IEnumerable<String>));

        // The arrays are the arguments of the methods that take the collections.
        String[] letters = new String[] { "a", "b", "c" };
        Console.WriteLine("generic " + CountOf(numbers) + " " + Join(letters) + " "
                                    + CountOf(new Int32[] { 1, 2 }));

        // The methods of System.Linq.
        Console.WriteLine("linq " + numbers.Count() + " " + numbers.Where(v => v > 1).Count() + " "
                                 + numbers.Max() + " " + numbers.Min() + " " + numbers.Skip(2).First());
        Console.WriteLine("select " + Join(numbers.OrderBy(v => v, ints).Select(v => "n" + v)));
        Console.WriteLine("orderby " + Join(numbers.OrderBy(v => v, ints)));
        Console.WriteLine("tolist " + numbers.ToList().Count);

        // The items are objects.
        Person[] people = new Person[] { new Person("ann", 10), new Person("bob", 20) };
        Console.WriteLine("objects " + people.Count() + " " + people.Last().Name);

        // The array is an Array<T>, its base type: it is read as an array of one dimension.
        Array<Int32> same = numbers;
        Console.WriteLine("base type " + same.Length + " " + (same[0] + 4));

        // An array that has no item.
        Int32[] empty = new Int32[0];
        Console.WriteLine("empty " + CountOf(empty) + " " + empty.Any());

        // The items are arrays.
        Int32[][] arrays = new Int32[][] { new Int32[] { 1 }, new Int32[] { 2, 3 } };
        Console.WriteLine("array of arrays " + arrays.Count() + " " + (arrays.Last().Count() + 1));

        Console.WriteLine("done");
    }
}
