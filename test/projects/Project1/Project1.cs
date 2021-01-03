
import System;

using System;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
		// Tuple<int, string> tuple = new Tuple<int, string>(10000, "Hello");

		int a = 10;
		long b = 20;

		long c = a + (long)b;
		Console.Write(c);
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

