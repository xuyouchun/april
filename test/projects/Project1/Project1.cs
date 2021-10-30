
import System;

using System;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
        MyStruct st = __CreateStruct(1, 2, 3);

        int r = st.A + st.B + st.C;
        Console.WriteLine(r);
    }
    
    private static MyStruct __CreateStruct(int a, int b, int c)
    {
        return new MyStruct(a, b, c);
    }

    private static int __Add(int a, int b, int c)
    {
        return a + b + c;
    }
};


struct MyStruct
{
    public MyStruct(int a, int b, int c)
    {
        A = a;
        B = b;
        C = c;
    }

	public int A;
    public int B;
    public int C;
    public int D;
    public int E;
    public int F;
    public int G;
};

class MyClass
{
    public MyClass()
    {

    }

    public MyStruct Field1;
    public int      Field2;
};

