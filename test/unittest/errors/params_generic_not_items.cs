/* EXPECT_ERROR only the items of a params generic parameter, or a tuple of them, can be passed with "..." */

import System;

using System;

static class Binder
{
    public static R Build<R, TArgs ...>(Delegate<R, TArgs ...> function, TArgs ... args)
        => function(1 ...);
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Console.WriteLine(Binder.Build((x, y) => x + y, 1, 2));
    }
}
