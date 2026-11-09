// Method.Invoke: static and instance methods, the arguments and the results that are boxed (numbers,
// strings) or references, methods of void, a virtual method that calls the implementation of the
// object, an exception that the method throws, and what happens when the call is wrong.

/* EXPECT

add 7
concat ann-bob
greet hi ann
ref Rex
void null
count 1
virtual Dog
base Animal
thrown Boom
no args 42
null target NullReferenceException
wrong target InvalidCastException
wrong arg InvalidCastException
arg count ArgumentException
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

class Animal
{
    public virtual String Kind() { return "Animal"; }
}

class Dog : Animal
{
    public override String Kind() { return "Dog"; }
}

class Tool
{
    public Int32 Calls;
    public String Owner = "ann";

    public static Int32 Add(Int32 a, Int32 b) { return a + b; }

    public static String Concat(String a, String b) { return a + "-" + b; }

    public static Int32 Answer() { return 42; }

    public String Greet(String word) { return word + " " + Owner; }

    public Pet MakePet(String name) { return new Pet(name); }

    public void Touch() { Calls = Calls + 1; }

    public static void Fail() { throw new InvalidOperationException("Boom"); }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Type type = Type.GetType(new Tool());
        Tool tool = new Tool();

        Object?[] ints = new Object?[2];
        ints[0] = 3;
        ints[1] = 4;
        Console.WriteLine("add " + type.GetMethod("Add", new Type[] { Type.GetType(1), Type.GetType(1) }).Invoke(null, ints));

        Object?[] strs = new Object?[2];
        strs[0] = "ann";
        strs[1] = "bob";
        Type str = Type.GetType("");
        Console.WriteLine("concat " + type.GetMethod("Concat", new Type[] { str, str }).Invoke(null, strs));

        Object?[] one = new Object?[1];
        one[0] = "hi";
        Console.WriteLine("greet " + type.GetMethod("Greet", new Type[] { str }).Invoke(tool, one));

        one[0] = "Rex";
        Pet pet = (Pet)type.GetMethod("MakePet", new Type[] { str }).Invoke(tool, one);
        Console.WriteLine("ref " + pet.Name);

        Object? nothing = type.GetMethod("Touch", new Type[0]).Invoke(tool, null);
        Console.WriteLine("void " + (nothing == null? "null" : "value"));
        Console.WriteLine("count " + tool.Calls);

        Animal animal = new Dog();
        Console.WriteLine("virtual " + Type.GetType(animal).BaseType.GetMethod("Kind", new Type[0]).Invoke(animal, null));
        Console.WriteLine("base " + Type.GetType(animal).BaseType.GetMethod("Kind", new Type[0]).Invoke(new Animal(), null));

        String thrown = "none";
        try { type.GetMethod("Fail", new Type[0]).Invoke(null, null); }
        catch (InvalidOperationException e) { thrown = e.Message; }
        Console.WriteLine("thrown " + thrown);

        Console.WriteLine("no args " + type.GetMethod("Answer", new Type[0]).Invoke(null, null));

        String error = "none";
        try { type.GetMethod("Touch", new Type[0]).Invoke(null, null); }
        catch (NullReferenceException e) { error = "NullReferenceException"; }
        Console.WriteLine("null target " + error);

        error = "none";
        try { type.GetMethod("Touch", new Type[0]).Invoke(new Pet("x"), null); }
        catch (InvalidCastException e) { error = "InvalidCastException"; }
        Console.WriteLine("wrong target " + error);

        error = "none";
        one[0] = 5;
        try { type.GetMethod("Greet", new Type[] { str }).Invoke(tool, one); }
        catch (InvalidCastException e) { error = "InvalidCastException"; }
        Console.WriteLine("wrong arg " + error);

        error = "none";
        try { type.GetMethod("Add", new Type[] { Type.GetType(1), Type.GetType(1) }).Invoke(null, one); }
        catch (ArgumentException e) { error = "ArgumentException"; }
        Console.WriteLine("arg count " + error);

        Console.WriteLine("done");
    }
}
