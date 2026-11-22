// A lambda that modifies a local variable of the method shares it with the method (and with the other lambdas): the
// variable is one, not a copy. A variable declared in a loop is a new one each time.

/* EXPECT

counter 3
shared 10 11
method sees 7
two lambdas 5 7
loop 1 2 3
nested 6
string ab!
done

*/

import System;

using System;
using System.Collections;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        // The lambda changes it, the method sees the change.
        Int32 count = 0;
        Delegate<Void> inc = () => { count++; };
        inc();
        inc();
        inc();
        Console.WriteLine("counter " + count);

        // The method changes it, the lambda sees the change.
        Int32 value = 10;
        Delegate<Int32> read = () => value;
        Int32 before = read();
        value = 11;
        Console.WriteLine("shared " + before + " " + read());

        // The lambda assigns it.
        Int32 set = 0;
        Delegate<Void, Int32> put = x => { set = x; };
        put(7);
        Console.WriteLine("method sees " + set);

        // Two lambdas share it.
        Int32 total = 0;
        Delegate<Void, Int32> add = x => { total += x; };
        Delegate<Int32> get = () => total;
        add(2);
        add(3);
        Int32 five = get();
        add(2);
        Console.WriteLine("two lambdas " + five + " " + get());

        // A variable of a loop is new in each turn.
        List<Delegate<Int32>> readers = new List<Delegate<Int32>>();
        for (Int32 i = 1; i <= 3; i++)
        {
            Int32 mine = i;
            mine = mine + 0;
            Delegate<Int32> r = () => mine;
            readers.Add(r);
        }

        Console.WriteLine("loop " + readers[0]() + " " + readers[1]() + " " + readers[2]());

        // A lambda in a lambda.
        Int32 deep = 1;
        Delegate<Void> outer = () => {
            Delegate<Void> innerLambda = () => { deep = deep + 5; };
            innerLambda();
        };

        outer();
        Console.WriteLine("nested " + deep);

        // Other types.
        String text = "a";
        Delegate<Void, String> append = s => { text = text + s; };
        append("b");
        append("!");
        Console.WriteLine("string " + text);

        Console.WriteLine("done");
    }
}
