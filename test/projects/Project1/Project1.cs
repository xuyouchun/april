import System;

using System;
using System.Diagnostics;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
        Delegate<Base> func = GetObj;

        Base obj = func();
        var r = obj.GetValue();

        Console.WriteLine(r);
    }

    static Base GetObj()
    {
        return new Base();
    }
}

class MyClass : Base
{
    private int Value
    {
        get { return 10; }
    }
}

