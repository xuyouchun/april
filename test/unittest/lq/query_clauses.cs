// The clauses of lq: more than one from, let, join, group, orderby with many keys, and into
// (a query that goes on with the result of a select or of a group).

/* EXPECT

from2 10,20,20,40
from3 111,121
let 12,20,30
let2 2,4
join ann@Rome,bob@Oslo,cy@Oslo,di@Rome
join+order bob,cy,ann
groupjoin Rome:2 Oslo:2
group 30:2 25:1 20:1
group count Oslo=2 Rome=2
into 6,8,10
orderby keys ann,cy,bob,di
many 1111111,2222222
done

*/

import System;

using System;
using System.Collections;
using System.Linq;

class Person
{
    public String Name;
    public Int32 Age;
    public Int32 City;

    public Person(String name, Int32 age, Int32 city)
    {
        this.Name = name;
        this.Age = age;
        this.City = city;
    }
}

class City
{
    public Int32 Id;
    public String Name;

    public City(Int32 id, String name)
    {
        this.Id = id;
        this.Name = name;
    }
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

    static String Join(IEnumerable<String> items)
    {
        String text = "";
        foreach (String item in items)
            text = text + (text == ""? "" : ",") + item;

        return text;
    }

    [EntryPoint]
    public static void Main()
    {
        Int32[] xs = new Int32[] { 5, 1, 4, 2, 3 };
        Int32[] ys = new Int32[] { 10, 20 };

        Console.WriteLine("from2 " + Join(``lq from x in xs from y in ys where x < 3 select x * y));
        Console.WriteLine("from3 " + Join(``lq from x in new Int32[] { 1 } from y in new Int32[] { 10, 20 }
                                           from z in new Int32[] { 100 } select x + y + z - 10 + 10));

        // let keeps the value with the range variable.
        Console.WriteLine("let " + Join(``lq from x in xs let sq = x * x where sq > 4 orderby sq select sq + x));
        Console.WriteLine("let2 " + Join(``lq from x in xs let a = x + 1 let b = a * 2 where b < 8 orderby b select b - 2 + x - x + 0));

        Person[] people = new Person[] {
            new Person("ann", 30, 1), new Person("bob", 25, 2),
            new Person("cy", 30, 2), new Person("di", 20, 1)
        };
        City[] cities = new City[] { new City(1, "Rome"), new City(2, "Oslo") };

        Console.WriteLine("join " + Join(``lq from p in people join c in cities on p.City equals c.Id
                                            select p.Name + "@" + c.Name));

        Console.WriteLine("join+order " + Join(``lq from p in people join c in cities on p.City equals c.Id
                                                 where p.Age > 20 orderby c.Name, p.Name select p.Name));

        String joined = "";
        foreach (String line in ``lq from c in cities join p in people on c.Id equals p.City into members
                                  select c.Name + ":" + members.Count())
            joined = joined + (joined == ""? "" : " ") + line;

        Console.WriteLine("groupjoin " + joined);

        String groups = "";
        foreach (IGrouping<Int32, Person> group in ``lq from p in people group p by p.Age)
            groups = groups + (groups == ""? "" : " ") + group.Key + ":" + group.Count();

        Console.WriteLine("group " + groups);

        String counts = "";
        foreach (String line in ``lq from p in people
                                  join c in cities on p.City equals c.Id
                                  group p by c.Name into g
                                  orderby g.Key
                                  select g.Key + "=" + g.Count())
            counts = counts + (counts == ""? "" : " ") + line;

        Console.WriteLine("group count " + counts);

        Console.WriteLine("into " + Join(``lq from x in xs select x * 2 into d where d > 4 orderby d select d));

        Console.WriteLine("orderby keys " + Join(``lq from p in people orderby p.Age descending, p.Name select p.Name));

        // More than six range variables: a pair of pairs, any count of them.
        Console.WriteLine("many " + Join(``lq from a in new Int32[] { 1, 2 }
                                           from b in new Int32[] { 10, 20 } where b == a * 10
                                           let c = a * 100 let d = a * 1000 let e = a * 10000
                                           let f = a * 100000 let g = a * 1000000
                                           select a + b + c + d + e + f + g - b + b));
        Console.WriteLine("done");
    }
}
