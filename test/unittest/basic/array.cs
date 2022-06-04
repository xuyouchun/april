// Multiple interfaces.

/* EXPECT

0, 1, 2, 3, 4
0, 1, 2, 3, 4
0, 1, 2, 3, 4, 5

*/

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        // Simple.
        {
            int[] arr = new int[5];

            arr[0] = 0;
            arr[1] = 1;
            arr[2] = 2;
            arr[3] = 3;
            arr[4] = 4;

            __PrintArray(arr);
        }

        // Array initialize.
        {
            int[] arr = new int[5] {
                0, 1, 2, 3, 4
            };

            __PrintArray(arr);
        }

        // High dimension.
        {
            int[,] arr = new int[3, 2];

            arr[0, 0] = 0;
            arr[0, 1] = 1;
            arr[1, 0] = 2;
            arr[1, 1] = 3;
            arr[2, 0] = 4;
            arr[2, 1] = 5;

            __PrintArray(arr);

        }

        // Multipy Array.
        {
            int[][] arr = new int[][] {
                new int[] { 1, 2 },
                new int[] { 3, 4, 5 },
                new int[] { 6, 7, 8, 9 }
            };

            // TODO:
            // __PrintArray(arr);
        }
    }

    private static void __PrintArray(Array<int> arr)
    {
        int len = arr.Length;

        if (len > 0)
        {
            Console.Write(arr[0]);

            for (int k = 1, len = arr.Length; k < len; k++)
            {
                Console.Write(", ");
                Console.Write(arr[k]);
            }
        }

        Console.WriteLine();
    }

    private static void __PrintArray(int[][] arr)
    {
        for (int index = 0, len = arr.Length; index < len; index++)
        {
            // __PrintArray(arr[index]);
        }
    }
}


