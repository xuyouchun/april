
import System;

using System;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine("Start ...");
        typedef MyStruct Type1;

        int a = 10, b = 200;
        Type1 obj1 = new Type1(a, b);
        // Type1 obj2 = obj1;

        // Console.WriteLine( obj1.A * 100 + obj1.B * 20 );
        Console.WriteLine( obj1.GetResult(1) - obj1.GetResult(2) );
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

    public int GetResult(int c)
    {
        return A + B + c;
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

