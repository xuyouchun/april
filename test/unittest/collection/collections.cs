// Lists, stacks and queues of System.Collections.Generic, and foreach over them.

/* EXPECT

10
9
100
1
77
b
10
2
1
x
1
range
150
10
20
40
50
a
b
10
20
0

*/

import System;

using System;
using System.Collections.Generic;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        {
            List<int> l = new List<int>();
            for (int i = 0; i < 10; i++)
                l.Add(i * i);
            Console.WriteLine(l.Count);
            Console.WriteLine(l[3]);
            l[3] = 100;
            Console.WriteLine(l[3]);
            l.RemoveAt(0);
            Console.WriteLine(l[0]);
            l.Insert(1, 77);
            Console.WriteLine(l[1]);
            List<String> s = new List<String>();
            s.Add("a");
            s.Add("b");
            Console.WriteLine(s[1]);
            int[] arr = l.ToArray();
            Console.WriteLine(arr.Length);
            Stack<int> st = new Stack<int>();
            st.Push(1);
            st.Push(2);
            Console.WriteLine(st.Pop());
            Console.WriteLine(st.Count);
            Queue<String> q = new Queue<String>();
            q.Enqueue("x");
            q.Enqueue("y");
            Console.WriteLine(q.Dequeue());
            Console.WriteLine(q.Count);
            try
            {
                l.RemoveAt(99);
            }
            catch (OutOfRangeException)
            {
                Console.WriteLine("range");
            }
        }

        {
            List<int> l = new List<int>();
            for (int i = 1; i <= 5; i++)
                l.Add(i * 10);
            int sum = 0;
            foreach (int x in l)
                sum += x;
            Console.WriteLine(sum);
            foreach (var y in l)
            {
                if (y == 30)
                    continue;
                Console.WriteLine(y);
            }
            List<String> s = new List<String>();
            s.Add("a");
            s.Add("b");
            foreach (String t in s)
                Console.WriteLine(t);
            foreach (int z in l)
            {
                if (z > 20)
                    break;
                Console.WriteLine(z);
            }
            int n = 0;
            foreach (int a in new List<int>())
                n++;
            Console.WriteLine(n);
        }
    }
}
