
import System;

using System;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
        /*
        int[,] arr = new int[,]
        {
            { 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 },
            { 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 },
            { 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 },
            { 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 },
        };
        */

        //int[] arr = new int[10];
        //arr.DoSomething();
        //arr.DoAnything();

        //StaticClass<long>.Call<int>(1, 2);

        //Class1 obj1 = new Class2();
        //int r = obj1.GetValue();

        //Console.Write("Hello world");

        int sum = 0;

        for(int k = 0; k < 100000000; k++)
        {
            sum += k;
        }

        Console.Print(sum);
    }
}

/*
static class StaticClass<T>
{
    public static void Call<K>(T t, K k)
    {
        Console.Print(123);
    }
};
*/

class Class1
{
    public abstract int GetValue();
}

class Class2 : Class1
{
    public override int GetValue()
    {
        return 20;
    }
};
