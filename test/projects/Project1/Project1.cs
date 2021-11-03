
import System;

using System;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
        long ticks = DateTime.UtcNow.Ticks;
        Console.WriteLine(ticks);
    }

    public static MyStruct __GetStruct(int a)
    {
        return new MyStruct(a);
    }
};


struct MyStruct
{
    public MyStruct(int a)
    {
        A = a;
    }

	public int A;

    public static MyStruct Instance
    {
        get { return new MyStruct(123); }
    }
};

class MyClass
{
    public MyClass()
    {

    }

    public MyStruct Field1;
    public int      Field2;
};

