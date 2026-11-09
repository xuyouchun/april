// Property.GetValue and Property.SetValue: the properties of an object, the static ones, the values
// that are boxed and the references, a property that cannot be read or written, and an indexer.

/* EXPECT

get 3 ann
set 9 bob
static 5
static set 6
auto 1
ref Rex
read only InvalidOperationException
write only InvalidOperationException
indexer InvalidOperationException
wrong value InvalidCastException
null object NullReferenceException
done

*/

import System;

using System;
using System.Reflection;

class Pet
{
    public String Name;

    public Pet(String name) { this.Name = name; }
}

class Owner
{
    private Int32 __age = 3;
    private String __name = "ann";
    private Int32 __secret;

    public Int32 Age { get { return __age; } set { __age = value; } }
    public String Name { get { return __name; } set { __name = value; } }
    public Int32 Level { get; set; }
    public Pet Pet { get { return new Pet("Rex"); } }
    public Int32 ReadOnly => 1;
    public Int32 WriteOnly { set { __secret = value; } }
    public static Int32 Count { get; set; }

    public Int32 this[Int32 index] { get { return index; } }
}

class Program
{
    static String Try(Property property, Object? obj, Boolean write, Object? value)
    {
        try
        {
            if (write)
                property.SetValue(obj, value);
            else
                property.GetValue(obj);
        }
        catch (InvalidOperationException e) { return "InvalidOperationException"; }
        catch (InvalidCastException e) { return "InvalidCastException"; }
        catch (NullReferenceException e) { return "NullReferenceException"; }

        return "none";
    }

    [EntryPoint]
    public static void Main()
    {
        Owner owner = new Owner();
        Type type = Type.GetType(owner);

        Console.WriteLine("get " + type.GetProperty("Age").GetValue(owner) + " "
                                 + type.GetProperty("Name").GetValue(owner));

        type.GetProperty("Age").SetValue(owner, 9);
        type.GetProperty("Name").SetValue(owner, "bob");
        Console.WriteLine("set " + owner.Age + " " + owner.Name);

        Owner.Count = 5;
        Console.WriteLine("static " + type.GetProperty("Count").GetValue(null));
        type.GetProperty("Count").SetValue(null, 6);
        Console.WriteLine("static set " + Owner.Count);

        type.GetProperty("Level").SetValue(owner, 1);
        Console.WriteLine("auto " + type.GetProperty("Level").GetValue(owner));

        Pet pet = (Pet)type.GetProperty("Pet").GetValue(owner);
        Console.WriteLine("ref " + pet.Name);

        Console.WriteLine("read only " + Try(type.GetProperty("ReadOnly"), owner, true, 1));
        Console.WriteLine("write only " + Try(type.GetProperty("WriteOnly"), owner, false, null));
        Console.WriteLine("indexer " + Try(type.GetProperty("Item"), owner, false, null));
        Console.WriteLine("wrong value " + Try(type.GetProperty("Age"), owner, true, "text"));
        Console.WriteLine("null object " + Try(type.GetProperty("Age"), null, false, null));

        Console.WriteLine("done");
    }
}
