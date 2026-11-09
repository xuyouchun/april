/* EXPECT_ERROR are many arguments, they are passed with ... */

import System;

using System;

static class Binder
{
    public static R Build<R, TArgs ...>(Delegate<R, TArgs ...> function, TArgs ... args)
        => function(args);
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine(Binder.Build((x, y) => x + y, 1, 2));
    }
}
