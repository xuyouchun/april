// System.Reflection: the fields, methods, properties, events and generic parameters that a type
// declares, their types, parameters and modifiers.

/* EXPECT

kind True False False False
base Animal Object
fields 5 Name String True False | Count Int32 False False | Limit Int32 True True | __ap_backing_Title String False False | __ap_backing_Changed MulticastDelegate<Void,Int32> False False
method Feed 2 Int32 Int32 amount 0 Boolean fast 1
constructor True 1
static False True
property Title String True True
property Size Int32 True False
accessor get_Title True
events 1 Changed Delegate<Void,Int32> add_Changed remove_Changed raise_Changed True True
generic Pair<A,B> 2 A 0 B 1
instance Pair<Int32,String> value Int32
overload 2 1
find True False
done

*/

import System;

using System;
using System.Reflection;
using Handler = System.Delegate<void, Int32>;

class Animal
{
}

class Dog : Animal
{
    public String Name;
    private Int32 Count;
    public readonly Int32 Limit = 3;

    public Dog(String name)
    {
        this.Name = name;
    }

    public Int32 Feed(Int32 amount, Boolean fast)
    {
        return amount;
    }

    public static void Reset()
    {
    }

    public void Feed(String food)
    {
    }

    public String Title { get; set; }

    public Int32 Size { get { return 1; } }

    public event Handler Changed;

}

class Pair<A, B>
{
    public A First;
    public B Second;
}

class Box<T>
{
    public T value;
}

class Program
{
    // The name without the hash that the compiler gives to what it makes (_ and 6 digits).
    static String NameOf(String name)
    {
        if (name.Length > 12 && name.Substring(0, 5) == "__ap_")
            return name.Substring(0, name.Length - 7);

        return name;
    }

    [EntryPoint]
    public static void Main()
    {
        Type dog = Type.GetType(new Dog("rex"));
        Console.WriteLine("kind " + dog.IsClass + " " + dog.IsStruct + " " + dog.IsInterface + " " + dog.IsGenericType);
        Console.WriteLine("base " + dog.BaseType.Name + " " + dog.BaseType.BaseType.Name);

        Array<Field> fields = dog.GetFields();
        String text = "fields " + fields.Length;
        for (Int32 k = 0; k < fields.Length; k++)
        {
            Field f = fields[k];
            text = text + (k == 0? " " : " | ") + NameOf(f.Name) + " " + f.FieldType.Name + " " + f.IsPublic + " " + f.IsReadOnly;
        }
        Console.WriteLine(text);

        Method feed = dog.GetMethod("Feed", new Type[] { typeof(Int32), typeof(Boolean) });
        Array<Parameter> ps = feed.GetParameters();
        Console.WriteLine("method " + feed.Name + " " + ps.Length + " " + feed.ReturnType.Name + " "
                          + ps[0].ParameterType.Name + " " + ps[0].Name + " " + ps[0].Position + " "
                          + ps[1].ParameterType.Name + " " + ps[1].Name + " " + ps[1].Position);

        Array<Method> all = dog.GetMethods();
        Boolean hasConstructor = false;
        for (Int32 k = 0; k < all.Length; k++)
            hasConstructor = hasConstructor || all[k].IsConstructor;
        Console.WriteLine("constructor " + hasConstructor + " " + dog.GetMethods("Reset").Length);

        Method reset = dog.GetMethods("Reset")[0];
        Console.WriteLine("static " + feed.IsStatic + " " + reset.IsStatic);

        Property title = dog.GetProperty("Title");
        Console.WriteLine("property " + title.Name + " " + title.PropertyType.Name + " " + title.CanRead + " " + title.CanWrite);

        Property size = dog.GetProperty("Size");
        Console.WriteLine("property " + size.Name + " " + size.PropertyType.Name + " " + size.CanRead + " " + size.CanWrite);
        Console.WriteLine("accessor " + title.GetMethod.Name + " " + title.GetMethod.IsPublic);

        Event changed = dog.GetEvent("Changed");
        Console.WriteLine("events " + dog.GetEvents().Length + " " + changed.Name + " "
                          + changed.EventType.Name + " " + changed.AddMethod.Name + " "
                          + changed.RemoveMethod.Name + " " + changed.RaiseMethod.Name + " "
                          + changed.RaiseMethod.IsPrivate + " " + changed.AddMethod.IsPublic);

        Type pair = typeof(Pair<Int32, String>);
        Array<GenericParameter> gps = pair.GetGenericParameters();
        Console.WriteLine("generic " + "Pair<A,B>" + " " + gps.Length + " " + gps[0].Name + " " + gps[0].Position
                          + " " + gps[1].Name + " " + gps[1].Position);

        Type box = typeof(Box<Int32>);
        Console.WriteLine("instance " + typeof(Pair<Int32, String>).Name.Substring(0, 4) + "<Int32,String> "
                          + box.GetFields()[0].Name + " " + box.GetFields()[0].FieldType.Name);

        Console.WriteLine("overload " + dog.GetMethods("Feed").Length + " " + dog.GetMethods("Reset").Length);
        Console.WriteLine("find " + (dog.GetField("Name") != null) + " " + (dog.GetField("Missing") != null));
        Console.WriteLine("done");
    }
}
