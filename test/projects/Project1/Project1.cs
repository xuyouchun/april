
import System;

using System;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
        MyClass obj = new MyClass();
        obj.Execute();
    }
};

class MyClass
{
    typedef System.Delegate<int, int, int> __Func;
    typedef System.Delegate<__Func> __FuncCreator;
    typedef System.Delegate<__FuncCreator> __FuncCreatorFactory;

    public void Execute()
    {
        var r = __GetFuncCreatorFactory()()()(1, 2) + 100 
            + __GetFuncCreatorFactory()()()(3, 4);
        Console.WriteLine(r);
    }

    public int Add(int a, int b)
    {
        return a + b;
    }

    public int Sub(int a, int b)
    {
        return a - b;
    }

    public int DoFunc(__Func func, int a, int b)
    {
        return func(a, b);
    }

    public __Func GetFunc()
    {
        return Add;
    }

    public __FuncCreator __GetFuncCreator()
    {
        return GetFunc;
    }

    public __FuncCreatorFactory __GetFuncCreatorFactory()
    {
        return __GetFuncCreator;
    }

    // public System.Delegate<

    public MyStruct GetStruct()
    {
        return new MyStruct(10, 20);
    }
};

struct MyStruct
{
    public MyStruct(int a, int b)
    {
        A = a;
        B = b;
        C = 1;
        D = 2;
    }

    public int A, B, C, D;
};

