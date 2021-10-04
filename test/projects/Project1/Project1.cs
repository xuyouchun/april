
import System;

using System;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine("Start ...");

        MyStruct obj1 = new MyStruct(1, 3);
        MyStruct obj2 = obj1;
        MyStruct obj3 = obj2;

        Console.WriteLine(
            obj2.A * 100 + obj3.B * 100
        );

        // __MyMethod( new MyStruct(10, 2), new MyStruct(20, 5) );
    }

    /*
    private static void __MyMethod(MyStruct obj1, MyStruct obj2)
    {
        Console.WriteLine(obj1.A + obj2.A);
    }
    */
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
