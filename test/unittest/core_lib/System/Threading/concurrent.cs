// System.Collections.Concurrent: the queue, the stack, the bag, the dictionary (with many threads), and BlockingCollection (a
// producer and consumers, a limit, CompleteAdding).

/* EXPECT

queue 4000 4000 0
stack 2000 True
bag 1000
dictionary 4008 4008 True
update 4000
blocking 5050 True
bounded True 2
timeout False True
enumerable 15

*/

import System;

using System;
using System.Collections;
using System.Collections.Concurrent;
using System.Threading;
using System.Threading.Tasks;

class Program
{
    static ConcurrentQueue<Int32> __queue = new ConcurrentQueue<Int32>();
    static ConcurrentStack<Int32> __stack = new ConcurrentStack<Int32>();
    static ConcurrentBag<Int32> __bag = new ConcurrentBag<Int32>();
    static ConcurrentDictionary<Int32, Int32> __dictionary = new ConcurrentDictionary<Int32, Int32>();
    static BlockingCollection<Int32> __blocking = new BlockingCollection<Int32>();
    static Int32 __dequeued;
    static Int32 __total;

    static Void Produce()
    {
        for (Int32 i = 0; i < 1000; i++)
        {
            __queue.Enqueue(i);
            __stack.Push(i);
            __bag.Add(i);
        }
    }

    static Void Consume()
    {
        Int32 value;
        while (__queue.TryDequeue(out value))
            Interlocked.Increment(ref __dequeued);
    }

    static Int32 Increment(Int32 key, Int32 value) { return value + 1; }

    static Void Fill(Object o)
    {
        Int32 id = (Int32)o;

        for (Int32 i = 0; i < 1000; i++)
        {
            __dictionary.AddOrUpdate(i % 8, 1, Increment);
            __dictionary.TryAdd(1000 + id * 1000 + i, i);
        }
    }

    static Void Take()
    {
        Int32 item;
        while (__blocking.TryTake(out item, -1))
            Interlocked.Add(ref __total, item);
    }

    [EntryPoint]
    public static void Main()
    {
        Thread[] producers = new Thread[4];
        for (Int32 i = 0; i < 4; i++)
        {
            producers[i] = new Thread(Produce);
            producers[i].Start();
        }

        for (Int32 i = 0; i < 4; i++)
            producers[i].Join();

        Int32 queued = __queue.Count;

        Thread[] consumers = new Thread[3];
        for (Int32 i = 0; i < 3; i++)
        {
            consumers[i] = new Thread(Consume);
            consumers[i].Start();
        }

        for (Int32 i = 0; i < 3; i++)
            consumers[i].Join();

        Console.WriteLine("queue " + queued + " " + __dequeued + " " + __queue.Count);

        Int32 popped = 0, top;
        for (Int32 i = 0; i < 2000; i++)
        {
            if (__stack.TryPop(out top))
                popped++;
        }

        Console.WriteLine("stack " + popped + " " + (__stack.Count == 2000));

        Int32 taken = 0, any;
        for (Int32 i = 0; i < 1000; i++)
        {
            if (__bag.TryTake(out any))
                taken++;
        }

        Console.WriteLine("bag " + taken);

        Delegate<Void, Object> fill = Fill;
        Thread[] fillers = new Thread[4];
        for (Int32 i = 0; i < 4; i++)
        {
            fillers[i] = new Thread(fill);
            fillers[i].Start(i);
        }

        for (Int32 i = 0; i < 4; i++)
            fillers[i].Join();

        Console.WriteLine("dictionary " + __dictionary.Keys.Length + " " + __dictionary.Count + " " + __dictionary.ContainsKey(0));

        Int32 sum = 0;
        for (Int32 k = 0; k < 8; k++)
            sum += __dictionary[k];

        Console.WriteLine("update " + sum);

        Thread[] takers = new Thread[3];
        for (Int32 i = 0; i < 3; i++)
        {
            takers[i] = new Thread(Take);
            takers[i].Start();
        }

        for (Int32 i = 1; i <= 100; i++)
            __blocking.Add(i);

        __blocking.CompleteAdding();

        for (Int32 i = 0; i < 3; i++)
            takers[i].Join();

        Console.WriteLine("blocking " + __total + " " + __blocking.IsCompleted);

        BlockingCollection<Int32> bounded = new BlockingCollection<Int32>(2);
        bounded.Add(1);
        bounded.Add(2);
        Boolean full = !bounded.TryAdd(3, 20);
        Int32 first = bounded.Take();
        Console.WriteLine("bounded " + full + " " + (first + bounded.Count));

        BlockingCollection<Int32> empty = new BlockingCollection<Int32>();
        Int32 none;
        Boolean got = empty.TryTake(out none, 20);
        empty.CompleteAdding();
        Boolean thrown = false;
        try { empty.Take(); } catch (InvalidOperationException e) { thrown = true; }
        Console.WriteLine("timeout " + got + " " + thrown);

        BlockingCollection<Int32> items = new BlockingCollection<Int32>();
        for (Int32 i = 1; i <= 5; i++)
            items.Add(i);

        items.CompleteAdding();

        Int32 total = 0;
        IEnumerator<Int32> consuming = items.GetConsumingEnumerable().GetEnumerator();
        while (consuming.MoveNext())
            total += consuming.Current;

        Console.WriteLine("enumerable " + total);
    }
}
