
import System;

using System;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
		Tuple<long, int, short> tuple = new Tuple<long, int, short>(123, 456, 789);
		// Tuple<int, int, int> tuple = new Tuple<int, int, int>(100, 200, 300);

		Console.WriteLine(tuple.Item1 + tuple.Item2 + tuple.Item3);
    }

    typedef System.Delegate<int, TArgs ...> Proc<TArgs ...>;
};


class MyClass
{
	public MyClass(int a)
	{
		// __a = a;
	}

	private int __a;

	public void Print()
	{
		// Console.Write(__a);
		// Console.WriteLine();
	}
}


/*
class MyClass1<TArgs ...>
{
	// public Delegate<void, TArgs ...> Func;
};
*/

