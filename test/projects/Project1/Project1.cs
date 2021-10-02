
import System;

using System;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
        // Console.WriteLine("start ...");
        // long a = 10, b = 20;

		MyStruct obj;
		// MyStruct obj = new MyStruct();
        // obj = new MyStruct(a, b);

        Console.WriteLine( obj.A + obj.B * 2 );
    }

};


struct MyStruct
{
    public MyStruct()
    {
        A = 1;
        B = 2;
    }

	public MyStruct(int a, int b)
	{
		A = a;
		B = b;
	}

	public int A;
	public int B;

    public int GetResult()
    {
        return A + B * 10;
    }
};

struct MyStruct2
{
    public MyStruct2(int a, int b)
    {
        A = a;
        B = b;
    }

    public int A;
    public int B;
}

/*
class MyClass
{
	public int Add(int a, int b)
	{
		return a + b;
	}

	public int Add(int a, int b, int c, int d)
	{
		return a + b + c + d;
	}

	public int Sub(int a, int b)
	{
		return a - b;
	}

	public int AddAll(int[] arr)
	{
		int sum = 0;
		for (int k = 0; k < arr.Length; k++)
		{
			sum += arr[k];
		}

		return sum;
	}
}
*/


/*
class MyClass1<TArgs ...>
{
	// public Delegate<void, TArgs ...> Func;
};
*/

