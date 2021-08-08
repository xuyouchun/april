
import System;

using System;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
		Tuple<int, long, short> tuple = new Tuple<int, long, short>(10, 20, 30);
		// Tuple<int> tuple = new Tuple<int>(10);

		Console.Write(tuple.Item1);
		Console.WriteLine();
		Console.Write(tuple.Item2);
		Console.WriteLine();
		Console.Write(tuple.Item3);
		Console.WriteLine();

		// Console.WriteLine();

		/*
		Console.Write(tuple.Item1);
		Console.WriteLine();
		*/
    }

    typedef System.Delegate<int, TArgs ...> Proc<TArgs ...>;
};


class MyClass
{
	public MyClass(int a)
	{
		
	}
}


/*
class MyClass1<TArgs ...>
{
	// public Delegate<void, TArgs ...> Func;
};
*/

