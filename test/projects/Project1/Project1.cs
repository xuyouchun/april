
import System;

using System;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
        TheDelegate delegateFunc = new TheDelegate();
        int ret = delegateFunc.Invoke(10);

        Console.Print(ret);
    }

    typedef System.Delegate<int, int> TheDelegate;
};


