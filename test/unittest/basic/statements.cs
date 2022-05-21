// Test statements.

/* EXPECT

5050
5050
5050
It's 3
3

*/

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        // While
        {
            int k = 1, sum = 0;
            while (k <= 100)
            {
                sum += k;
                k++;
            }

            Console.WriteLine(sum);
        }

        // Do while.
        {
            int k = 1, sum = 0;
            do
            {
                sum += k;
                k++;

            } while (k <= 100);

            Console.WriteLine(sum);
        }

        // For.
        {
            int sum = 0;
            for (int k = 1; k <= 100; k++)
            {
                sum += k;
            }

            Console.WriteLine(sum);
        }

        // Switch.
        {
            int k = 3;
            switch (k)
            {
                case 1:
                    Console.WriteLine("It's 1");
                    break;

                case 2:
                    Console.WriteLine("It's 2");
                    break;

                case 3:
                    Console.WriteLine("It's 3");
                    break;

                default:
                    Console.WriteLine("I'm not sure");
                    break;
            }
        }

        // Switch pass through.
        {
            int k = 1;
            int sum = 0;

            switch (k)
            {
                case 0:
                    sum++;

                case 1:
                    sum++;

                case 2:
                    sum++;

                default:
                    sum++;
            }

            Console.WriteLine(sum);
        }
    }
}

