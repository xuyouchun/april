// What the compiler makes is named __ap_<usage>_<detail>_<hash>, with a hash of 6 hexadecimal digits
// that is made of the usage, the owner (the type, the method with the types of its params) and the
// detail (the serial, or the name that the member is made for): the same code gives the same names.
// The test shows the names without the hash, that depends on the name of the file. What the compiler
// makes has the attribute CompilerGenerated: the state class of an iterator is a nest type of
// the type that has the method, the lambdas are methods of it, the fields and the accessors of the
// automatic properties and of the events too. The reflection does not treat them in another way:
// they are listed and found by name like the others.

/* EXPECT

type Host
  field __ap_backing_Count 1
  field __ap_backing_Changed 1
  method Numbers 0
  method Apply 0
  method Apply 0
  method Start 0
  method __ap_move_0 1
  method __ap_lambda_0 1
  method __ap_lambda_0 1
  method get_Count 1
  method set_Count 1
  method add_Changed 1
  method remove_Changed 1
  method raise_Changed 1
  method Host 1
  property Count 1
type Host.__ap_iterator_0
  field __ap_state 1
  field __ap_current 1
  field __ap_delegate 1
  field limit 1
  field __ap_local_1_k 1
  method __ap_iterator_0 1
  method MoveNext 1
  method Reset 1
  method Dispose 1
  method __ap_finish 1
  method GetEnumerator 1
  method get_Current 1
  property Current 1
12
Runtime.CompilerServices.CompilerGeneratedAttribute
True

7

*/

import System;

using System;
using System.Collections;
using System.Reflection;
using System.Runtime.CompilerServices;

typedef Handler = System.Delegate<void, Int32>;

class Host
{
    public Int32 Count { get; set; }
    public event Handler Changed;

    public IEnumerable<Int32> Numbers(Int32 limit)
    {
        Int32 k = 0;
        while (k < limit)
            yield return k++;
    }

    public Int32 Apply(Int32 x)
    {
        Int32 y = 10;
        Delegate<Int32, Int32> add = a => a + y + x;
        return add(1);
    }

    public Int32 Apply(String s)
    {
        Delegate<Int32, Int32> twice = a => a * 2;
        return twice(2);
    }

    public Object Start() { return Numbers(2); }
}

class Program
{
    // The name without its hash: _ and 6 digits.
    static String Show(String name)
    {
        for (Int32 k = 0; k + 12 <= name.Length; k++)
        {
            if (name.Substring(k, 5) == "__ap_")
                return name.Substring(0, name.Length - 7);
        }

        return name;
    }

    static void Dump(Type type)
    {
        Console.WriteLine("type " + Show(type.Name));
        foreach (Field f in type.GetFields())
            Console.WriteLine("  field " + Show(f.Name) + " " + f.GetAttributes().Length);
        foreach (Method m in type.GetMethods())
            Console.WriteLine("  method " + Show(m.Name) + " " + m.GetAttributes().Length);
        foreach (Property p in type.GetProperties())
            Console.WriteLine("  property " + Show(p.Name) + " " + p.GetAttributes().Length);
    }

    [EntryPoint]
    public static void Main()
    {
        Host h = new Host();
        Dump(typeof(Host));
        Dump(Type.GetType(h.Start()));
        Console.WriteLine(h.Apply(1));

        // The attribute is an ordinary one, and the fields are found by name like the others.
        Field backing = null;
        foreach (Field f in typeof(Host).GetFields())
        {
            if (Show(f.Name) == "__ap_backing_Count")
                backing = f;
        }

        Console.WriteLine(Type.GetType(backing.GetAttributes()[0]).FullName);
        Console.WriteLine(typeof(Host).GetField(backing.Name) != null);
        Console.WriteLine(backing.Name.Length - Show(backing.Name).Length);
    }
}
