
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
        obj1 = new MyStruct(30, 40);
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

