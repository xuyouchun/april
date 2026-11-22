// $"..." gives the same text as String.Format of the same items: numbers, chars, bools, enums, objects, null, formats, alignments.

/* EXPECT

True True True True True True True True True True True True True True
done

*/

import System;

using System;

enum Color
{
    Red,
    Green,
}

class Thing : IStringable
{
    public String ToString() => "thing";
}

class Money : IFormattable
{
    public String ToString(String? format) => "money:" + format;

    public String ToString(String? format, IFormatProvider? provider) => "money:" + format;

    public String ToString() => "money";
}

struct Point
{
    public Int32 X;
}

class Program
{
    static Boolean Same(String interpolated, String formatted)
    {
        if (interpolated != formatted)
            Console.WriteLine("DIFFERENT [" + interpolated + "] [" + formatted + "]");

        return interpolated == formatted;
    }

    [EntryPoint]
    public static void Main()
    {
        Int32 i = -42;
        Int64 big = 9000000000;
        UInt8 byteValue = 200;
        Double d = 1234.5678;
        Single f = 0.25f;
        Char c = 'z';
        Boolean flag = false;
        String text = "text";
        String? none = null;
        Color color = Color.Green;
        Object thing = new Thing();
        Object? nothing = null;
        Money money = new Money();
        Point point = new Point();

        String r = "";
        r += Same($"{i} {big} {byteValue} {d} {f} {c} {flag}", String.Format("{0} {1} {2} {3} {4} {5} {6}", i, big, byteValue, d, f, c, flag)) + " ";
        r += Same($"{text}|{none}|{nothing}|{thing}|{color}", String.Format("{0}|{1}|{2}|{3}|{4}", text, none, nothing, thing, color)) + " ";
        r += Same($"{i,6}|{i,-6}|{text,8}|{text,-8}|{flag,7}", String.Format("{0,6}|{0,-6}|{1,8}|{1,-8}|{2,7}", i, text, flag)) + " ";
        r += Same($"{d:F2} {d:N1} {i:D5} {big:X} {f:P0} {d:E2}", String.Format("{0:F2} {0:N1} {1:D5} {2:X} {3:P0} {0:E2}", d, i, big, f)) + " ";
        r += Same($"{money:fmt} {money} {thing:x} {color:G}", String.Format("{0:fmt} {0} {1:x} {2:G}", money, thing, color)) + " ";
        r += Same($"{d,12:F1}|{i,-8:D4}|", String.Format("{0,12:F1}|{1,-8:D4}|", d, i)) + " ";
        r += Same($"{{{i}}}", String.Format("{{{0}}}", i)) + " ";
        r += Same($"{point.X}{i + 1}{(flag? "y" : "n")}", String.Format("{0}{1}{2}", point.X, i + 1, flag? "y" : "n")) + " ";
        r += Same($"{i}", i.ToString()) + " ";
        r += Same($"a{i}", "a" + i) + " ";
        r += Same($"{text}{text}{text}{text}{text}{text}{text}{text}{text}{text}", text + text + text + text + text + text + text + text + text + text) + " ";
        r += Same($"{d:F3}{i:D2}", d.ToString("F3") + i.ToString("D2")) + " ";
        r += Same($"{(Object)point}", String.Format("{0}", (Object)point)) + " ";
        r += Same($"", "") + " ";

        Console.WriteLine(r.Trim());
        Console.WriteLine("done");
    }
}
