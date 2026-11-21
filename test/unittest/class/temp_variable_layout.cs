// The temporary variables of the compiler (the structs that a property or a call returns, whose member is read) do not
// take the place of the variables of the blocks: in a block that is followed by another block, with variables of other
// types, the temporary variables were put over the variables of the first one.

/* EXPECT

first 75 -15 abc
second True 21 abc
results ok21 thrown ok33
done

*/

import System;

using System;

struct Span4
{
    public Int64 A;
    public Int64 B;
    public Int64 C;
    public Int64 D;
    public Int64 E;
    public Int64 F;

    public Span4(Int64 a) { A = a; B = a + 1; C = a + 2; D = a + 3; E = a + 4; F = a + 5; }

    public Int64 Sum => A + B + C + D + E + F;
}

class Source
{
    public Source(String name) { Name = name; }

    public String Name;

    public Span4 Value => new Span4(10);

    public static Span4 Limit => new Span4(-5);

    public static Span4 Make(Int64 a) { return new Span4(a); }

    public Boolean Has(String s) { return s.Length > 0; }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        {
            String text = "abc";
            Console.WriteLine("first " + new Source("a").Value.Sum + " " + Source.Limit.Sum + " " + text);
            Console.WriteLine("second " + new Source("b").Has(text) + " " + Source.Make(1).Sum + " " + text);
        }

        {
            String[] results = new String[3];
            Span4[] values = new Span4[] { Source.Make(1), Source.Make(2), Source.Make(3) };
            for (Int32 i = 0; i < values.Length; i++)
            {
                try
                {
                    if (values[i].A == 2)
                        throw new OutOfRangeException();

                    results[i] = "ok" + values[i].Sum;
                }
                catch (OutOfRangeException)
                {
                    results[i] = "thrown";
                }
            }

            Console.WriteLine("results " + results[0] + " " + results[1] + " " + results[2]);
        }

        Console.WriteLine("done");
    }
}
