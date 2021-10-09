
import System;

using System;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine("Start ...");

        MyStruct obj1 = new MyStruct(10, 20);
        __MyMethod(obj1);
    }

    private static void __MyMethod(MyStruct obj1)
    {
        Console.WriteLine("__MyMethod");
        MyStruct obj2 = new MyStruct(50, 60);
        obj1 = obj2;

        Console.WriteLine(obj2.A + obj2.B);
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
};

class MyClass
{
    public MyClass()
    {

    }

    MyStruct Field1;
    int      Field2;
};

