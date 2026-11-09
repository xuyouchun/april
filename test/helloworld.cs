import System;

using System;
using System.Collections;
using System.Linq;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        int[] arr = new int[] { 1, 2, 3, 4, 5, 6, 7 };

        // var result = arr.Where(x => x % 2 == 0).ToArray();
        var result = ``lq from item in arr where item % 2 == 0 orderby item descending select item;
        PrintArray(result);
    }

    private static void PrintArray(IEnumerable<int> arr)
    {
        foreach (int i in arr)
        {
            Console.WriteLine(i);
        }
    }
}

