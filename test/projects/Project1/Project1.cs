
import System;

using System;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
        int[] arr = StaticClass.CreateArray<int>(1, 2, 3, 4, 5);
        Console.Print( StaticClass.Sum(arr) );
    }
}

class StaticClass
{
    public static T[] CreateArray<T>(T t1, T t2, T t3, T t4, T t5)
    {
        return new T[] { t1, t2, t3, t4, t5 };
    }

    public static int Sum(int[] arr)
    {
        int sum = 0;
        for(int k = 0, len = arr.Length; k < len; k++)
        {
            sum += arr[k];
        }

        return sum;
    }
};

