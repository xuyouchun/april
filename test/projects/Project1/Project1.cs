
import System;

using System;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine("Start ...");

        MyClass obj = new MyClass();
        obj.Field1 = new MyStruct(1, 2);
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
};

