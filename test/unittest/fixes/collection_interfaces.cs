// Generic collection interfaces with variance: IEnumerable<out T>, IList<T>, IComparer<in T>...

/* EXPECT

2
2
dog;dog;
2
0
3
7
True

2
True

2
9
8
1
8
9
False

needs comparer
2
a
b
True

True

dog
*/
import System;
using System;
using System.Collections.Generic;
class Animal { public virtual String Name() { return "animal"; } }
class Dog : Animal { public override String Name() { return "dog"; } }
class Cat : Animal { public override String Name() { return "cat"; } }
class ByName : IComparer<Animal> {
  public Int32 Compare(Animal x, Animal y) { return new StringComparer().Compare(x.Name(), y.Name()); }
}
class Program {
 static Int32 CountAll(IEnumerable<Animal> animals) { Int32 n = 0; foreach (Animal a in animals) n++; return n; }
 static String Names(IReadOnlyList<Animal> list) {
   String r = "";
   for (Int32 i = 0; i < list.Count; i++) r += list[i].Name() + ";";
   return r;
 }
 [EntryPoint] public static void Main() {
 List<Dog> dogs = new List<Dog>(); dogs.Add(new Dog()); dogs.Add(new Dog());
 IEnumerable<Animal> animals = dogs;                 // covariance
 Console.WriteLine(CountAll(animals));
 Console.WriteLine(CountAll(dogs));
 Console.WriteLine(Names(dogs));
 IReadOnlyCollection<Object> objs = dogs; Console.WriteLine(objs.Count);
 IComparer<Dog> cmp = new ByName();                  // contravariance
 Console.WriteLine(cmp.Compare(new Dog(), new Dog()));
 IList<Int32> ints = new List<Int32>(new Int32Comparer());
 ints.Add(5); ints.Add(7); ints.Add(9);
 Console.WriteLine(ints.Count); Console.WriteLine(ints[1]);
 Console.WriteLine(ints.Contains(7)); Console.WriteLine(ints.IndexOf(9)); Console.WriteLine(ints.Remove(5)); Console.WriteLine(ints.Count);
 ints.Insert(0, 1); ints[1] = 8; Console.WriteLine(ints[0] + ints[1]);
 Int32[] copy = new Int32[4]; ints.CopyTo(copy, 1); Console.WriteLine(copy[2]);
 foreach (Int32 v in ints) Console.WriteLine(v);
 ICollection<Int32> c = ints; Console.WriteLine(c.IsReadOnly);
 List<Int32> plain = new List<Int32>(); plain.Add(1);
 try { plain.Contains(1); } catch (InvalidOperationException) { Console.WriteLine("needs comparer"); }
 Stack<Dog> st = new Stack<Dog>(); st.Push(new Dog()); st.Push(new Dog());
 IEnumerable<Animal> sa = st; Console.WriteLine(CountAll(sa));
 Queue<String> q = new Queue<String>(); q.Enqueue("a"); q.Enqueue("b");
 IEnumerable<String> qs = q; foreach (String s in qs) Console.WriteLine(s);
 StringComparer sc = new StringComparer(); Console.WriteLine(sc.Equals("x", "x")); Console.WriteLine(sc.GetHashCode("x") == sc.GetHashCode("x"));
 IEnumerator<Animal> e = animals.GetEnumerator(); e.MoveNext(); Console.WriteLine(e.Current.Name());
} }
