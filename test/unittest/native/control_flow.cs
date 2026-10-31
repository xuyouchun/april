// Loops, branches, short circuit operators and recursion.

/* EXPECT

55
10
25
8
9
True

False

True

False

True

3628800
610
True

False

-1
0
1

*/

import System;

using System;

class Program
{
    static int Factorial(int n)
    {
        if (n <= 1)
            return 1;

        return n * Factorial(n - 1);
    }

    static int Fibonacci(int n)
    {
        if (n < 2)
            return n;

        return Fibonacci(n - 1) + Fibonacci(n - 2);
    }

    static int Max(int a, int b)
    {
        return a > b? a : b;
    }

    static bool IsPrime(int n)
    {
        if (n < 2)
            return false;

        for (int k = 2; k * k <= n; k++)
        {
            if (n % k == 0)
                return false;
        }

        return true;
    }

    static int Sign(int n)
    {
        if (n > 0)
            return 1;
        else if (n < 0)
            return -1;

        return 0;
    }

    [EntryPoint]
    public static void Main()
    {
        // While.
        {
            int sum = 0, k = 1;
            while (k <= 10)
            {
                sum = sum + k;
                k++;
            }

            Console.WriteLine(sum);
        }

        // For and nested loops.
        {
            int count = 0;
            for (int i = 0; i < 5; i++)
            {
                for (int j = 0; j < i; j++)
                    count++;
            }

            Console.WriteLine(count);
        }

        // Do while and break / continue.
        {
            int n = 0, s = 0;
            do
            {
                n++;
                if (n % 2 == 0)
                    continue;

                if (n > 9)
                    break;

                s = s + n;
            }
            while (n < 100);

            Console.WriteLine(s);
        }

        // Conditional expression, and short circuit operators.
        {
            Console.WriteLine(Max(3, 8));
            Console.WriteLine(Max(9, 8));

            int a = 5, b = 0;
            Console.WriteLine(a > 3 && b == 0);
            Console.WriteLine(a > 3 && b != 0);
            Console.WriteLine(a < 3 || b == 0);
            Console.WriteLine(a < 3 || b != 0);
            Console.WriteLine(!(a < 3));
        }

        // Recursion and calls.
        {
            Console.WriteLine(Factorial(10));
            Console.WriteLine(Fibonacci(15));
            Console.WriteLine(IsPrime(97));
            Console.WriteLine(IsPrime(91));
            Console.WriteLine(Sign(-7));
            Console.WriteLine(Sign(0));
            Console.WriteLine(Sign(12));
        }
    }
}
