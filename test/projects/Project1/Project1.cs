
import System;

using System;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
		// Tuple<int, string> tuple = new Tuple<int, string>(100, "Hello World");
		// new Tuple<int>(100);

		int a = 1, b = 2;
		if (a + b != 3)
		{
			Console.WriteLine("Yes!");
		}
		else
		{
			Console.WriteLine("NO!");
		}

		// Console.Write(tuple.Item2);
        // Console.WriteLine("Hello World");
    }

    typedef System.Delegate<int, TArgs ...> Proc<TArgs ...>;
};


class MyClass
{

}


/*
class MyClass1<TArgs ...>
{
	// public Delegate<void, TArgs ...> Func;
};
*/

