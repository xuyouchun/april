// Char is 16 bits (UTF-16) on every platform: a character that is not in the basic plane is two Chars of a
// String (a pair of surrogates), a Char is stored in 2 bytes in the arrays, the fields and the structs.

/* EXPECT

len 5
astral 2
half 1
equal True
concat 7
bmp 20013
surrogates 55357 56832
next y
order True
array 3 20013
field 8 120
struct 3 99 7
sum 97111
text a1+中

*/

import System;

using System;

class Holder
{
    public Char First;
    public Int32 Number;
    public Char Last;
}

struct Pair
{
    public Char Letter;
    public Int32 Count;
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        // A character that is not in the basic plane is a pair of surrogates in a string.
        String text = "a😀z中";
        Console.WriteLine("len " + text.Length);
        Console.WriteLine("astral " + "😀".Length);
        Console.WriteLine("half " + "😀".Substring(0, 1).Length);
        Console.WriteLine("equal " + (text.Substring(1, 2) == "😀"));
        Console.WriteLine("concat " + (text + "xy").Length);

        // The values of the characters.
        Char cn = '中';
        Console.WriteLine("bmp " + (Int32)cn);

        Char high = '\uD83D';
        Char low = '\uDE00';
        Console.WriteLine("surrogates " + (Int32)high + " " + (Int32)low);

        Char c = 'x';
        Char next = (Char)((Int32)c + 1);
        Console.WriteLine("next " + next);
        Console.WriteLine("order " + (high < low && c < next));

        // Arrays, fields and structs have a Char of 2 bytes.
        Char[] chars = new Char[] { 'a', 'b', '中' };
        Console.WriteLine("array " + chars.Length + " " + (Int32)chars[2]);

        Holder holder = new Holder();
        holder.First = 'x';
        holder.Number = 8;
        holder.Last = 'x';
        Console.WriteLine("field " + holder.Number + " " + (Int32)holder.Last);

        Pair[] pairs = new Pair[3];
        pairs[2].Letter = 'c';
        pairs[2].Count = 7;
        Console.WriteLine("struct " + pairs.Length + " " + (Int32)pairs[2].Letter + " " + pairs[2].Count);

        Int32 sum = 0;
        for (Int32 k = 0; k < chars.Length - 1; k++)
            sum = sum * 1000 + (Int32)chars[k] % 1000 + 3 * k;

        Console.WriteLine("sum " + (sum + 10));

        // A Char in a concatenation.
        Console.WriteLine("text " + chars[0] + 1 + "+" + chars[2].ToString().Substring(0, 1));
    }
}
