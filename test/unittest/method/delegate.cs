// Delegate.

/* EXPECT

__MyFunc
300
300
300

*/

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        // Simple delegate.
        {
            var func = __MyFunc;
            func();
        }

        // With arguments and return value.
        {
            var func = __MyFunc2;
            int r = func(100, 200);

            Console.WriteLine(r);
        }

        // Pass as arguments.
        {
            var func = __MyFunc2;
            var r = __Call(func, 100, 200);

            Console.WriteLine(r);
        }

        // Return a delegate.
        {
            var func = __GetFunc();
            var r = func(100, 200);

            Console.WriteLine(r);
        }
    }

    private static void __MyFunc()
    {
        Console.WriteLine("__MyFunc");
    }

    private static int __MyFunc2(int a, int b)
    {
        return a + b;
    }

    private static int __Call(Delegate<int, int, int> func, int a, int b)
    {
        return func(a, b);
    }

    private static Delegate<int, int, int> __GetFunc()
    {
        return __MyFunc2;
    }
}


