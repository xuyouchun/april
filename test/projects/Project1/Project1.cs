
import System;

using System;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
		// Tuple<int, string> tuple = new Tuple<int, string>(10000, "Hello");

		// int a = 100;
		// int b = 200;
		// long c = a + b;

		double f1 = 111.111, f2 = 222.222;
		float f = (float)f1 + (float)f2;

		Console.Write(f);
		Console.WriteLine();
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

