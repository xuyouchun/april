// Benchmark: strings, a StringBuilder and the concatenation.

import System;

using System;
using System.Text;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        StringBuilder builder = new StringBuilder();
        for (Int32 i = 0; i < 400000; i++)
        {
            builder.Append("item");
            builder.Append(i);
            builder.Append(',');
        }

        String text = builder.ToString();
        Int32 commas = 0;
        for (Int32 i = 0; i < text.Length; i++)
        {
            if (text[i] == ',')
                commas++;
        }

        String joined = "";
        for (Int32 i = 0; i < 4000; i++)
            joined = joined + i;

        Console.WriteLine(text.Length + " " + commas + " " + joined.Length);
    }
}
