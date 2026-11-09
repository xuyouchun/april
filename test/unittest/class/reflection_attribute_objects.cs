// The arguments of an attribute that are not numbers or strings by their parameters: an Object
// parameter takes a number (in the type that it was written: 5 is an Int32, 2.5 a Double, true a
// Boolean), a string, or null; a Type parameter takes typeof(A); a field of type Object that is
// named ([Mark(Extra = 7)]) takes them too; an array of numbers or of strings is an argument
// (new Int32[] { 1, 2, 3 }). The attribute classes are not made by the program.

/* EXPECT

int 5 Int32
double 2.5 Double
bool True Boolean
string text String
null True
type A
named Int32 7 B
two 1 Int32 s String
array 3 1 3
names 2 a bb
done

*/

import System;

using System;
using System.Reflection;

class ObjAttribute : Attribute
{
    public Object Value;

    public ObjAttribute(Object value) { Value = value; }
}

class KindAttribute : Attribute
{
    public Type Of;

    public KindAttribute(Type of) { Of = of; }
}

class MarkAttribute : Attribute
{
    public Object Extra;
    public Type Other;

    public MarkAttribute() { }
}

class PairAttribute : Attribute
{
    public Object First;
    public Object Second;

    public PairAttribute(Object first, Object second) { First = first; Second = second; }
}

class ListAttribute : Attribute
{
    public Array<Int32> Items;
    public Array<String> Names;

    public ListAttribute(Array<Int32> items) { Items = items; }
}

class A { }
class B { }

[Obj(5)]
class IntOwner { }

[Obj(2.5)]
class DoubleOwner { }

[Obj(true)]
class BoolOwner { }

[Obj("text")]
class StringOwner { }

[Obj(null)]
class NullOwner { }

[Kind(typeof(A))]
class TypeOwner { }

[Mark(Extra = 7, Other = typeof(B))]
class NamedOwner { }

[Pair(1, "s")]
class PairOwner { }

[List(new Int32[] { 1, 2, 3 }, Names = new String[] { "a", "bb" })]
class ListOwner { }

class Program
{
    static String Describe(Object value)
    {
        return value + " " + Type.GetType(value).Name;
    }

    [EntryPoint]
    public static void Main()
    {
        Object[] keep = new Object[] { new IntOwner(), new DoubleOwner(), new BoolOwner(),
            new StringOwner(), new NullOwner(), new TypeOwner(), new NamedOwner(), new PairOwner(), new ListOwner() };

        Console.WriteLine("int " + Describe(((ObjAttribute)typeof(IntOwner).GetAttributes()[0]).Value));
        Console.WriteLine("double " + Describe(((ObjAttribute)typeof(DoubleOwner).GetAttributes()[0]).Value));
        Console.WriteLine("bool " + Describe(((ObjAttribute)typeof(BoolOwner).GetAttributes()[0]).Value));
        Console.WriteLine("string " + Describe(((ObjAttribute)typeof(StringOwner).GetAttributes()[0]).Value));
        Console.WriteLine("null " + (((ObjAttribute)typeof(NullOwner).GetAttributes()[0]).Value == null));
        Console.WriteLine("type " + ((KindAttribute)typeof(TypeOwner).GetAttributes()[0]).Of.Name);

        MarkAttribute mark = (MarkAttribute)typeof(NamedOwner).GetAttributes()[0];
        Console.WriteLine("named " + Type.GetType(mark.Extra).Name + " " + mark.Extra + " " + mark.Other.Name);

        PairAttribute pair = (PairAttribute)typeof(PairOwner).GetAttributes()[0];
        Console.WriteLine("two " + Describe(pair.First) + " " + Describe(pair.Second));

        ListAttribute list = (ListAttribute)typeof(ListOwner).GetAttributes()[0];
        Console.WriteLine("array " + list.Items.Length + " " + list.Items[0] + " " + list.Items[2]);
        Console.WriteLine("names " + list.Names.Length + " " + list.Names[0] + " " + list.Names[1]);
        Console.WriteLine("done");
    }
}
