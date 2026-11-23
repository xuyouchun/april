// Events: a field-like event keeps its handlers (a MulticastDelegate that is read only, a new one
// is made by each += and -=, null when there is none), E += handler and E -= handler call the add
// and remove methods, E(args) calls the raise method: the handlers are called in the order that they
// were added in, nothing is done when there is none. A static event, handlers that are lambdas, and
// the result of the last handler.

/* EXPECT

nobody
A1
B1
B2
twice A3
twice A3
A3
lambda 4
static 7
reread 2 True
result 12
no result 0
done

*/

import System;

using System;
typedef Handler = System.Delegate<void, Int32>;
typedef Adder = System.Delegate<Int32, Int32>;

class Button
{
    public event Handler Clicked;

    public static event Handler Pressed;

    public event Adder Computed;

    public void Click(Int32 x)
    {
        Clicked(x);
    }

    public static void Press(Int32 x)
    {
        Pressed(x);
    }

    public Int32 Compute(Int32 x)
    {
        return Computed(x);
    }

    public Int32 Count()
    {
        return Clicked == null? 0 : Clicked.Count;
    }
}

class Counter
{
    public Int32 Total;
}

class Program
{
    static void A(Int32 x) { Console.WriteLine("A" + x); }
    static void B(Int32 x) { Console.WriteLine("B" + x); }
    static void Show(Int32 x) { Console.WriteLine("static " + x); }
    static Int32 One(Int32 x) { return x + 1; }
    static Int32 Two(Int32 x) { return x + 2; }

    [EntryPoint]
    public static void Main()
    {
        Button button = new Button();

        Console.WriteLine("nobody");
        button.Click(0);

        button.Clicked += A;
        button.Clicked += B;
        button.Click(1);

        button.Clicked -= A;
        button.Click(2);

        // The same method added twice is called twice; -= removes the last one.
        Button other = new Button();
        other.Clicked += A;
        other.Clicked += A;
        other.Clicked -= A;
        Console.Write("twice ");
        other.Click(3);
        other.Clicked += A;
        other.Clicked -= B;
        Console.Write("twice ");
        other.Click(3);

        Counter counter = new Counter();
        Button third = new Button();
        third.Clicked += x => { counter.Total = counter.Total + x; };
        third.Click(1);
        third.Click(3);
        Console.WriteLine("lambda " + counter.Total);

        Button.Pressed += Show;
        Button.Press(7);

        // The list is read: a handler that is added is a new list.
        Console.WriteLine("reread " + other.Count() + " " + (button.Count() == 1));

        button.Computed += One;
        button.Computed += Two;
        Console.WriteLine("result " + button.Compute(10));

        Button empty = new Button();
        Console.WriteLine("no result " + empty.Compute(10));

        Console.WriteLine("done");
    }
}
