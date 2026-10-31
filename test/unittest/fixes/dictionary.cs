// Dictionary<TKey, TValue>: a hash table, with the comparer of the keys.

/* EXPECT

3
25
26
True

False

True

30
False

True

False

2
missing
duplicate
100
v77
50
60
n205
False

4545
one
60
Pat
0
five
*/
import System;
using System;
using System.Collections.Generic;

class Person { public String Name; public Person(String n) { Name = n; } }

class Program {
 [EntryPoint] public static void Main() {
  Dictionary<String, Int32> ages = new Dictionary<String, Int32>(new StringComparer());
  ages.Add("ann", 30); ages.Add("bob", 25); ages["cy"] = 41;
  Console.WriteLine(ages.Count);
  Console.WriteLine(ages["bob"]);
  ages["bob"] = 26;
  Console.WriteLine(ages["bob"]);
  Console.WriteLine(ages.ContainsKey("cy"));
  Console.WriteLine(ages.ContainsKey("zed"));
  Int32 v;
  Console.WriteLine(ages.TryGetValue("ann", out v));
  Console.WriteLine(v);
  Console.WriteLine(ages.TryGetValue("none", out v));
  Console.WriteLine(ages.Remove("ann"));
  Console.WriteLine(ages.Remove("ann"));
  Console.WriteLine(ages.Count);
  try { Int32 x = ages["gone"]; } catch (KeyNotFoundException e) { Console.WriteLine("missing"); }
  try { ages.Add("bob", 1); } catch (InvalidOperationException e) { Console.WriteLine("duplicate"); }
  // growth and reuse of the removed slots.
  Dictionary<Int32, String> big = new Dictionary<Int32, String>(new Int32Comparer());
  for (Int32 i = 0; i < 100; i++) big.Add(i, "v" + i);
  Console.WriteLine(big.Count);
  Console.WriteLine(big[77]);
  for (Int32 i = 0; i < 100; i += 2) big.Remove(i);
  Console.WriteLine(big.Count);
  for (Int32 i = 200; i < 210; i++) big[i] = "n" + i;
  Console.WriteLine(big.Count);
  Console.WriteLine(big[205]);
  Console.WriteLine(big.ContainsKey(50));
  Int32 sum = 0;
  foreach (KeyValuePair<Int32, String> kv in big) sum += kv.Key;
  Console.WriteLine(sum);
  IDictionary<Int32, String> d = big;
  d.Set(1, "one");
  Console.WriteLine(d[1]);
  Console.WriteLine(d.Keys.Count);
  Dictionary<String, Person> people = new Dictionary<String, Person>(new StringComparer());
  people.Add("p", new Person("Pat"));
  Console.WriteLine(people["p"].Name);
  big.Clear();
  Console.WriteLine(big.Count);
  big.Add(5, "five");
  Console.WriteLine(big[5]);
 }
}
