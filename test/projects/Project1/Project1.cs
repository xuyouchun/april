
import System;

using System;
using System.Reflection;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
        System.Delegate<int, int> proc = Delegate.Create<int, int>(new MethodInfo(), null);
    }

    typedef System.Delegate<int, params TArgs> Proc<params TArgs>;
};

