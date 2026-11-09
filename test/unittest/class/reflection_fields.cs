// Field.GetValue and Field.SetValue: the fields of an object, the static fields, the values that are
// boxed (numbers, structs, strings) and the references, and what happens when the object or the
// value is wrong.

/* EXPECT

get 3 ann 2.5 True
get ref Rex
set 9 bob 7.5 False
set ref Max
static 5 text
static set 6 changed
struct 1 2
struct set 8 2
null name True
base 4
object null NullReferenceException
wrong object InvalidCastException
wrong value InvalidCastException
wrong number InvalidCastException
done

*/

import System;

using System;
using System.Reflection;

struct Point
{
    public Int32 X;
    public Int32 Y;
}

class Pet
{
    public String Name;

    public Pet(String name) { this.Name = name; }
}

class Animal
{
    public Int32 Legs = 4;
}

class Owner : Animal
{
    public Int32 Age = 3;
    public String Name = "ann";
    public Double Score = 2.5;
    public Boolean Active = true;
    public Pet Pet;
    public Point Where;
    public static Int32 Count = 5;
    public static String Label = "text";

    public Owner() { this.Pet = new Pet("Rex"); }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Owner owner = new Owner();
        Type type = Type.GetType(owner);

        Console.WriteLine("get " + type.GetField("Age").GetValue(owner) + " "
                                 + type.GetField("Name").GetValue(owner) + " "
                                 + type.GetField("Score").GetValue(owner) + " "
                                 + type.GetField("Active").GetValue(owner));

        Pet pet = (Pet)type.GetField("Pet").GetValue(owner);
        Console.WriteLine("get ref " + pet.Name);

        type.GetField("Age").SetValue(owner, 9);
        type.GetField("Name").SetValue(owner, "bob");
        type.GetField("Score").SetValue(owner, (Double)7.5);
        type.GetField("Active").SetValue(owner, false);
        Console.WriteLine("set " + owner.Age + " " + owner.Name + " " + owner.Score + " " + owner.Active);

        type.GetField("Pet").SetValue(owner, new Pet("Max"));
        Console.WriteLine("set ref " + owner.Pet.Name);

        Console.WriteLine("static " + type.GetField("Count").GetValue(null) + " "
                                    + type.GetField("Label").GetValue(null));
        type.GetField("Count").SetValue(null, 6);
        type.GetField("Label").SetValue(null, "changed");
        Console.WriteLine("static set " + Owner.Count + " " + Owner.Label);

        owner.Where.X = 1;
        owner.Where.Y = 2;
        Point where = (Point)type.GetField("Where").GetValue(owner);
        Console.WriteLine("struct " + where.X + " " + where.Y);
        where.X = 8;
        type.GetField("Where").SetValue(owner, where);
        Console.WriteLine("struct set " + owner.Where.X + " " + owner.Where.Y);

        type.GetField("Name").SetValue(owner, null);
        Console.WriteLine("null name " + (owner.Name == null));

        Console.WriteLine("base " + Type.GetType(owner).BaseType.GetField("Legs").GetValue(owner));

        String error = "none";
        try { type.GetField("Age").GetValue(null); }
        catch (NullReferenceException e) { error = "NullReferenceException"; }
        Console.WriteLine("object null " + error);

        error = "none";
        try { type.GetField("Age").GetValue(new Pet("x")); }
        catch (InvalidCastException e) { error = "InvalidCastException"; }
        Console.WriteLine("wrong object " + error);

        error = "none";
        try { type.GetField("Pet").SetValue(owner, "text"); }
        catch (InvalidCastException e) { error = "InvalidCastException"; }
        Console.WriteLine("wrong value " + error);

        error = "none";
        try { type.GetField("Age").SetValue(owner, (Int64)3); }
        catch (InvalidCastException e) { error = "InvalidCastException"; }
        Console.WriteLine("wrong number " + error);

        Console.WriteLine("done");
    }
}
