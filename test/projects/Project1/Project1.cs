
import System;

using System;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine("Start ...");

        MyStruct obj1 = new MyStruct(10, 20);
        MyStruct obj2 = new MyStruct(30, 40);

        __MyMethod( obj1, obj2 );
    }

    private static void __MyMethod(MyStruct obj1, MyStruct obj2)
    {
        int a1 = obj1.A, b1 = obj1.B;
        int a2 = obj2.A, b2 = obj2.B;

        Console.WriteLine(a1 + b1 + a2 + b2);
    }
};


struct MyStruct
{
	public MyStruct(int a, int b)
	{
		A = a;
		B = b;
	}

	public int A;
	public int B;

    public int GetResult(int c)
    {
        return A + B + c;
    }
};

/*

class MyClass
{
    public MyClass()
    {
        obj = new MyStruct(1, 2);
    }

    MyStruct obj;
};

*/
