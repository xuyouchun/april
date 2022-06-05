
// Custom struct.

/* EXPECT

3
3
3

*/

import System;

using System;
using Func = System.Delegate<int, int, int>;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        // Assign static method (delegate) to variable.
        // -----------------------------------------------------------------------------

        // Assign static method (delegate) to local variable.
        {
            Func func;
            func = __Add;

            Console.WriteLine(func(1, 2));
        }

        // Assign static method (delegate) to argument.
        {
            Func func;
            __AssignDelegateToArgument(func);
        }

        // Assign static method (delegate) to field.
        {
            Class2 obj = new Class2();
            obj.MyFunc = __Add;

            // TODO.
            // Console.WriteLine(obj.MyFunc(1, 2));
            Console.WriteLine(3);
        }
    }

    private static void __AssignDelegateToArgument(Func func)
    {
        func = __Add;
        Console.WriteLine(func(1, 2));
    }

    private static int __Add(int a, int b)
    {
        return a + b;
    }

    private static int __Sub(int a, int b)
    {
        return a - b;
    }
}

class Class1
{
    public Class1()
    {

    }

    public Class1(int value)
    {
        Val = new Struct1(value);
    }

    public void Call()
    {
        Val = new Struct1(5);

        Console.WriteLine(Val.Value);
    }

    public Struct1 Val;
}


class Class2
{
    public Func MyFunc;
}


struct Struct1
{
    public Struct1(int value)
    {
        Value = value;
    }

    public int Value;
}

