// Events of a generic type: the delegate type is made of the arguments of the type; the handlers
// of two instances of the type are not shared.

/* EXPECT

int 5
int 7
text a
other 1 0
done

*/

import System;

using System;

class Box<T>
{
    public event Delegate<void, T> Changed;

    private T __value;

    public void Set(T value)
    {
        __value = value;
        Changed(value);
    }

    public Int32 Count => Changed == null? 0 : Changed.Count;
}

class Program
{
    static void Show(Int32 x) { Console.WriteLine("int " + x); }
    static void ShowText(String x) { Console.WriteLine("text " + x); }

    [EntryPoint]
    public static void Main()
    {
        Box<Int32> numbers = new Box<Int32>();
        numbers.Changed += Show;
        numbers.Set(5);
        numbers.Set(7);

        Box<String> texts = new Box<String>();
        texts.Changed += ShowText;
        texts.Set("a");

        Box<Int32> other = new Box<Int32>();
        other.Changed += Show;
        other.Changed -= Show;
        Box<Int32> counted = new Box<Int32>();
        counted.Changed += Show;
        Console.WriteLine("other " + counted.Count + " " + other.Count);
        Console.WriteLine("done");
    }
}
