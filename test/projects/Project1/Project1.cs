
import System;

using System;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
        var a = 0xFFFFFFFF, b = 2, c = a + b;   // a is UInt32, b is Int32, c is Int64
        var r = a > 0 && b > 0;                 // r is Boolean

        if (r)
            Console.WriteLine("Hello True!");
        else
            Console.WriteLine("Hello False!");
    }

    private static int __Add(MyStruct st1, MyStruct st2)
    {
        return st1.A + st2.A;
    }

    public static MyStruct __GetStruct(MyStruct st)
    {
        return new MyStruct(-st.A);
    }

    public static void __Print(MyStruct st)
    {
        Console.WriteLine(st.A);
    }
};


struct MyStruct
{
    public MyStruct(int a)
    {
        A = a;
    }

	public int A;

    public int GetA()
    {
        return A;
    }

    public MyStruct GetInner()
    {
        return new MyStruct(A + 1);
    }

    public MyStruct Inner
    {
        get { return new MyStruct(A + 2); }
    }

    public MyStruct Inner2
    {
        get { return new MyStruct(A + 3); }
    }

    public static MyStruct Instance
    {
        get { return new MyStruct(123); }
    }

    public static MyStruct operator + (MyStruct st1, MyStruct st2)
    {
        return new MyStruct(st1.A + st2.A);
    }

    public static MyStruct operator - (MyStruct st)
    {
        return new MyStruct(-st.A);
    }

    public static MyStruct Minus(MyStruct st)
    {
        return new MyStruct(-st.A);
    }

    public static int operator - (MyStruct st1, MyStruct st2)
    {
        return st1.A - st2.A;
    }
};

class MyClass
{
    public MyClass()
    {

    }

    public MyStruct Field1;
    public int      Field2;
};

