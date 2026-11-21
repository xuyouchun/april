// The condition expression (c? a : b) with the value of a generic param: the variables, a call of a delegate, default(T), as the
// result of a method and as the value of a variable, for numbers, strings and classes.

/* EXPECT

pick 2 1 b a 6 5
call 42 7
default 42
default 0
default x
nested 3 1

*/

import System;

using System;

class Item
{
    public Int32 N;
}

class Holder<T>
{
    public Delegate<T> Make;
    public Boolean Has;

    public T Pick(T a, T b, Boolean first) { return first? a : b; }

    public T FromMake(T other) { return Has? Make() : other; }

    public T OrDefault() { T value = Has? Make() : default(T); return value; }

    public T Nested(T a, T b, T c, Int32 which) { return which == 0? a : which == 1? b : c; }
}

class Program
{
    static Int32 MakeInt() { return 42; }

    static String MakeString() { return "x"; }

    [EntryPoint]
    public static void Main()
    {
        Holder<Int32> numbers = new Holder<Int32>();
        Holder<String> strings = new Holder<String>();
        Holder<Item> items = new Holder<Item>();

        Item five = new Item(), six = new Item();
        five.N = 5;
        six.N = 6;

        Console.WriteLine("pick " + numbers.Pick(1, 2, false) + " " + numbers.Pick(1, 2, true) + " " + strings.Pick("a", "b", false)
                          + " " + strings.Pick("a", "b", true) + " " + items.Pick(five, six, false).N + " " + items.Pick(five, six, true).N);

        numbers.Make = MakeInt;
        numbers.Has = true;
        Console.WriteLine("call " + numbers.FromMake(0) + " " + (numbers.Has? 7 : 8));

        Console.WriteLine("default " + numbers.OrDefault());
        numbers.Has = false;
        Console.WriteLine("default " + numbers.OrDefault());

        strings.Make = MakeString;
        strings.Has = true;
        Console.WriteLine("default " + strings.OrDefault());

        Console.WriteLine("nested " + numbers.Nested(1, 2, 3, 2) + " " + numbers.Nested(1, 2, 3, 0));
    }
}
