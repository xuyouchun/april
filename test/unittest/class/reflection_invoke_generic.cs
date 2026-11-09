// Method.Invoke of the methods of a class of the library (Stack<T>), of the instances of a generic
// type (the parameters and the results are those of its arguments), and a virtual method of a
// generic class that a class overrides.

/* EXPECT

push 2
count 2
pop 20
peek 10
text hello
virtual Derived 5
base Base<Int32> 5
done

*/

import System;

using System;
using System.Collections;
using System.Reflection;

class Base<T>
{
    public T Value;

    public virtual String Describe() { return "Base<Int32> " + Value; }
}

class Derived : Base<Int32>
{
    public override String Describe() { return "Derived " + this.Value; }
}

class Holder<T>
{
    public T Item;

    public Holder(T item) { Item = item; }

    public T Get() { return Item; }

    public Void Set(T item) { Item = item; }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Stack<Int32> stack = new Stack<Int32>();
        Type type = Type.GetType(stack);
        Type number = Type.GetType(1);

        Method push = type.GetMethod("Push", new Type[] { number });
        Object?[] args = new Object?[1];

        args[0] = 10;
        push.Invoke(stack, args);
        args[0] = 20;
        push.Invoke(stack, args);
        Console.WriteLine("push " + stack.Count);

        Console.WriteLine("count " + type.GetProperty("Count").GetValue(stack));
        Console.WriteLine("pop " + type.GetMethod("Pop", new Type[0]).Invoke(stack, null));
        Console.WriteLine("peek " + type.GetMethod("Peek", new Type[0]).Invoke(stack, null));

        Holder<String> holder = new Holder<String>("a");
        Type holderType = Type.GetType(holder);
        Object?[] text = new Object?[1];
        text[0] = "hello";
        holderType.GetMethod("Set", new Type[] { Type.GetType("") }).Invoke(holder, text);
        Console.WriteLine("text " + holderType.GetMethod("Get", new Type[0]).Invoke(holder, null));

        Base<Int32> derived = new Derived();
        derived.Value = 5;
        Console.WriteLine("virtual " + Type.GetType(derived).BaseType.GetMethod("Describe", new Type[0]).Invoke(derived, null));

        Base<Int32> plain = new Base<Int32>();
        plain.Value = 5;
        Console.WriteLine("base " + Type.GetType(plain).GetMethod("Describe", new Type[0]).Invoke(plain, null));

        Console.WriteLine("done");
    }
}
