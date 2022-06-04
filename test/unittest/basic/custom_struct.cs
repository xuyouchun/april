// Custom struct.

/* EXPECT

1
2
3

4
5
6

3
3
3

100
101
102

201
202
203

301
302
303

401
402
403

501
502
503

601
602
603

*/

import System;

using System;
using Func = System.Delegate<int, int, int>;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        // Assign new expression to a variable.
        // -----------------------------------------------------------------------------

        // Assign new expression to local variable.
        {
            Struct1 val = new Struct1(1);

            Console.WriteLine(val.Value);
        }

        // Assign new expression to argument.
        {
            Struct1 val;
            __AssignNewToArgument(val);
        }

        // Assign new expression to field.
        {
            Class1 obj = new Class1();
            obj.Val = new Struct1(3);

            Console.WriteLine(obj.Val.Value);
        }

        // Return to a variable.
        // -----------------------------------------------------------------------------

        // Return to local variable.
        {
            Struct1 val = __GetStruct1(4);

            Console.WriteLine(val.Value);
        }

        // Return to argument.
        {
            Struct1 val;
            __ReturnToArgument(val);
        }

        // Return to field.
        {
            Class1 obj = new Class1();
            obj.Val = __GetStruct1(6);

            Console.WriteLine(obj.Val.Value);
        }

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

        // Assign local variable to variable.
        // -----------------------------------------------------------------------------

        // Assign local variable to local variable.
        {
            Struct1 val = new Struct1(100);
            Struct1 val2;

            val2 = val;
            Console.WriteLine(val2.Value);
        }

        // Assign local variable to param variable.
        {
            Struct1 val;
            __AssignLocalToParam(val);
        }

        // Assign local variable to field variable.
        {
            Class1 obj = new Class1();
            obj.Val = new Struct1(102);

            Console.WriteLine(obj.Val.Value);
        }

        // Assign field variable to local variable.
        // -----------------------------------------------------------------------------

        // Assign field variable to local variable.
        {
            Class1 obj1 = new Class1(201);
            Struct1 val1;

            val1 = obj1.Val;
            Console.WriteLine(val1.Value);
        }

        // Assign field variable to param variable.
        {
            Struct1 val;
            __AssignFieldToParam(val);
        }

        // Assign field variable to field variable.
        {
            Class1 obj1 = new Class1(203);
            Class1 obj2 = new Class1();

            obj2.Val = obj1.Val;
            Console.WriteLine(obj2.Val.Value);
        }

        // Assign param variable to variable.
        // -----------------------------------------------------------------------------

        // Assign param variable to local variable.
        {
            Struct1 val = new Struct1(301);

            __AssignParamToLocal(val);
        }

        // Assign param variable to param variable.
        {
            Struct1 val1, val2;
            val1 = new Struct1(302);

            __AssignParamToParam(val1, val2);
        }

        // Assign param variable to field variable.
        {
            Struct1 val = new Struct1(303);

            __AssignParamToLocal(val);
        }

        // Return a custom struct.
        // -----------------------------------------------------------------------------

        // Return a local variable.
        {
            Struct1 val = __ReturnLocal();
            Console.WriteLine(val.Value);
        }

        // Return a argument variable.
        {
            Struct1 val = new Struct1(402);
            Struct1 val1 = __ReturnParam(val);

            Console.WriteLine(val1.Value);
        }

        // Return a field variable.
        {
            Struct1 val = __ReturnField();
            Console.WriteLine(val.Value);
        }

        // Pass an argument.
        // -----------------------------------------------------------------------------

        // Pass a local variable as argument.
        {
            Struct1 val = new Struct1(501);
            __ParseLocalAsArgument(val);
        }

        // Pass an argument variable as argument.
        {
            Struct1 val = new Struct1(502);
            __ParseArgumentAsArgument(val);
        }

        // Pass a file variable as argument.
        {
            Class1 obj1 = new Class1(503);
            __ParseFieldAsArgument(obj1.Val);
        }

        // ?: operator.
        // -----------------------------------------------------------------------------

        // Simple.
        {
            Struct1 val1 = new Struct1(601);
            Struct1 val2 = new Struct1(602);

            Struct1 val = true? val1 : val2;
            Console.WriteLine(val.Value);
        }

        // Complex.
        {
            Struct1 val1 = new Struct1(604);

            Struct1 val = true? __GetStruct1(602) :
                          true? new Struct1(603) : val1;

            Console.WriteLine(val.Value);
        }

        // Delegate
        {
            Delegate<int, int, int> func = true? __Add : __Sub;
            int v = func(600, 3);
            Console.WriteLine(v);
        }
    }

    private static void __AssignNewToArgument(Struct1 val)
    {
        val = new Struct1(2);
        Console.WriteLine(val.Value);
    }

    private static Struct1 __GetStruct1(int value)
    {
        return new Struct1(value);
    }

    private static void __ReturnToArgument(Struct1 val)
    {
        val = new Struct1(5);
        Console.WriteLine(val.Value);
    }

    private static int __Add(int a, int b)
    {
        return a + b;
    }

    private static void __AssignDelegateToArgument(Func func)
    {
        func = __Add;
        Console.WriteLine(func(1, 2));
    }

    private static void __AssignLocalToParam(Struct1 val)
    {
        Struct1 val2 = new Struct1(101);
        val = val2;

        Console.WriteLine(val.Value);
    }

    private static void __AssignFieldToParam(Struct1 val)
    {
        Class1 obj1 = new Class1(202);
        val = obj1.Val;

        Console.WriteLine(val.Value);
    }

    private static void __AssignParamToLocal(Struct1 val)
    {
        Struct1 val1;

        val1 = val;
        Console.WriteLine(val1.Value);
    }

    private static void __AssignParamToParam(Struct1 val1, Struct1 val2)
    {
        val2 = val1;
        Console.WriteLine(val2.Value);
    }

    private static void __AssignParamToField(Struct1 val)
    {
        Class1 obj1 = new Class1();
        obj1.Val = val;

        Console.WriteLine(obj1.Val.Value);
    }

    private static Struct1 __ReturnLocal()
    {
        return new Struct1(401);
    }

    private static Struct1 __ReturnParam(Struct1 val)
    {
        return val;
    }

    private static Struct1 __ReturnField()
    {
        Class1 obj1 = new Class1(403);
        return obj1.Val;
    }

    private static void __ParseLocalAsArgument(Struct1 val)
    {
        Console.WriteLine(val.Value);
    }

    private static void __ParseArgumentAsArgument(Struct1 val)
    {
        __ParseArgumentAsArgument_2(val);
    }

    private static void __ParseArgumentAsArgument_2(Struct1 val)
    {
        Console.WriteLine(val.Value);
    }

    private static void __ParseFieldAsArgument(Struct1 val)
    {
        Console.WriteLine(val.Value);
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

