
import System;

using System;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
		System.Delegate<int, int, int, int> func = obj.Add;

		// int sum = obj.Add(3, 2) + obj.Sub(9, 1);
		// int sum = func(2, 3);
		// Console.WriteLine(sum);
    }
};


class MyClass
{
	public int Add(int a, int b)
	{
		return a + b;
	}

	public int Add(int a, int b, int c)
	{
		return a + b + c;
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

