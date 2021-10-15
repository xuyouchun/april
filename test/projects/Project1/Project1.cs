
import System;

using System;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
        double c = Math.Log(100) + Math.Sin(15);

        Console.WriteLine(c);
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

    public MyStruct Field1;
    public int      Field2;
};

