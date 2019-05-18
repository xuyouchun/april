
import System;

using System;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
        Proc<int> proc;
        int ret = proc.Invoke(10);

        Console.Print(ret);
    }

    typedef System.Delegate<int, params TArgs> Proc<params TArgs>;
};


