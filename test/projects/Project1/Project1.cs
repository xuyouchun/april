
import System;

using System;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
        for(int k = 0; k < 10; k++)
        {
            switch(k)
            {
                case 1:
                    Console.WriteLine("1");
                    break;

                case 2:
                    Console.WriteLine("2");
                    break;

                default:
                    Console.WriteLine("Others");
                    break;
            }
        }
    }
};

class Dictionary<TKey, TValue>
{

};
