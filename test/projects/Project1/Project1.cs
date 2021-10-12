
import System;

using System;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
        int a = 1, b = 2;
        long c = a + b;

        Console.WriteLine(c);
    }

    private static void __MyMethod(MyStruct obj1)
    {

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

