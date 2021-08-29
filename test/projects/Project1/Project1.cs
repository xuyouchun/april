
import System;

using System;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
		MyClass obj = new MyClass();

		typedef System.Delegate<int, int, int> Func;
		Func func = obj.Add;

		int sum = func(1, 2);

		Console.WriteLine(sum);
    }
};


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


/*
class MyClass1<TArgs ...>
{
	// public Delegate<void, TArgs ...> Func;
};
*/

