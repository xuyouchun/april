
import System;

using System;
using System.Diagnostics;

class Project1
{
    [EntryPoint, Trace]
    public static void Main()
    {
        for (int v = 2; v <= 100; v++)
        {
            bool prime = true;

            for (int k = 2, end = Math.Sqrt(v); k <= end; k++)
            {
                if (v % k == 0)
                {
                    prime = false;
                    break;
                }
            }

            if (prime)
                Console.WriteLine(v);
        }
    }
};

struct MyStruct
{
    public MyStruct(int value)
    {
        Value = value;
    }

    public int Value;

    public MyStruct GetStruct()
    {
        return new MyStruct(Value + 1);
    }
};
