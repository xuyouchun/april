// The constructor of Object has nothing to run: it is made and invoked by reflection (and by new T() of T = Object) in
// every back end.

/* EXPECT

ctor True
invoke null
new True
maker True
plain 4

*/

import System;

using System;
using System.Reflection;

class Maker<T> where T : new()
{
    public T Make() { return new T(); }
}

class Plain
{
    public Int32 A = 4;
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Type type = typeof(Object);
        Array<Method> methods = type.GetMethods();

        for (Int32 k = 0; k < methods.Length; k++)
        {
            Method method = methods[k];

            if (method.IsConstructor && method.GetParameters().Length == 0)
            {
                Object made = method.NewInstance(new Object?[0]);
                Console.WriteLine("ctor " + (made != null));

                Object?[] none = new Object?[0];
                Object? result = method.Invoke(made, none);
                if (result == null)
                    Console.WriteLine("invoke null");
            }
        }

        Console.WriteLine("new " + (Activator.CreateInstance<Object>() != null));
        Console.WriteLine("maker " + (new Maker<Object>().Make() != null));

        Plain plain = Activator.CreateInstance<Plain>();
        Console.WriteLine("plain " + plain.A);
    }
}
