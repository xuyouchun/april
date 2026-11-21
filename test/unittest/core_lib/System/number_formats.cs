// The formats of the numbers: percent and currency, the custom formats (0 # . , % 'text' ;), the symbols of a
// NumberFormatInfo, String.Format with a provider, Console.WriteLine(format, args), the methods of a number literal.

/* EXPECT

percent 12.34% 50% 100.00% -25.6%
currency $1,234.50 -$1,234.50 $5 $0.00
fixed 1234.57 1,234.57 1,235 2.5 2.55 3.0 .5 [] 007 12345 005
percent2 26% 12.3% 0.5%
text (555) 123-4567 ID:5 5x #7
sections 5 (5) zero +3 -3
negative -1,234.5 -0.0 -12
integers 1,234,567 5.00 1,234 1,234,567.50
provider 1.234.567,89 1.234.567,89 €1.234.567,89 ~5 50 % 1,5 1.234,50
infos True True False True True caught caught
1-2
literals ff 3.5 255 5 3.14 2
caught caught caught caught caught caught
single 3.14 1.2

*/

import System;

using System;
using System.Globalization;
using System.Text;

class Program
{
    static String Try(String format, Double value)
    {
        try
        {
            return value.ToString(format);
        }
        catch (FormatException)
        {
            return "caught";
        }
    }

    [EntryPoint]
    public static void Main()
    {
        Double ratio = 0.1234, half = 0.5, quarter = -0.256, money = 1234.5, negative = -1234.5;
        Int32 one = 1, five = 5;
        Console.WriteLine("percent " + ratio.ToString("P") + " " + half.ToString("P0") + " " + one.ToString("P") + " " + quarter.ToString("P1"));
        Console.WriteLine("currency " + money.ToString("C") + " " + negative.ToString("C") + " " + five.ToString("C0") + " " + (0.0).ToString("C"));

        Double value = 1234.5678, small = 2.5, medium = 2.55, three = 3.0, fraction = 0.5, zero = 0.0;
        Int32 seven = 7, big = 12345;
        Console.WriteLine("fixed " + value.ToString("0.00") + " " + value.ToString("#,##0.00") + " " + value.ToString("#,##0")
            + " " + small.ToString("0.0#") + " " + medium.ToString("0.0#") + " " + three.ToString("0.0#") + " " + fraction.ToString("#.##")
            + " [" + zero.ToString("#.##") + "] " + seven.ToString("000") + " " + big.ToString("00") + " " + five.ToString("0#0"));

        Double percent = 0.256, tenth = 0.1234, small2 = 0.005;
        Console.WriteLine("percent2 " + percent.ToString("0%") + " " + tenth.ToString("0.0%") + " " + small2.ToString("0.0%"));

        Int64 phone = 5551234567;
        Console.WriteLine("text " + phone.ToString("(000) 000-0000") + " " + five.ToString("'ID:'0") + " " + five.ToString("0\"x\"")
            + " " + seven.ToString("\\#0"));

        Int32 minus = -5, none = 0, plus = 3, minusThree = -3;
        Console.WriteLine("sections " + five.ToString("0;(0);zero") + " " + minus.ToString("0;(0);zero") + " " + none.ToString("0;(0);zero")
            + " " + plus.ToString("+0;-0") + " " + minusThree.ToString("+0;-0"));

        Double tiny = -0.04, negativeInt = -12.0;
        Console.WriteLine("negative " + negative.ToString("#,##0.0") + " " + tiny.ToString("0.0") + " " + negativeInt.ToString("0"));

        Int32 millions = 1234567, thousand = 1234;
        Double dollars = 1234567.5;
        Console.WriteLine("integers " + millions.ToString("#,##0") + " " + five.ToString("0.00") + " " + thousand.ToString("0,000")
            + " " + dollars.ToString("#,##0.00"));

        NumberFormatInfo european = NumberFormatInfo.CurrentInfo.Clone();
        european.NumberDecimalSeparator = ",";
        european.NumberGroupSeparator = ".";
        european.CurrencySymbol = "€";
        european.NegativeSign = "~";
        european.PercentSymbol = " %";

        Double large = 1234567.891, onePointFive = 1.5, price = 1234.5;
        Console.WriteLine("provider " + large.ToString("N2", european) + " " + large.ToString("#,##0.00", european) + " " + large.ToString("C", european)
            + " " + minus.ToString("D", european)
            + " " + half.ToString("P0", european) + " " + onePointFive.ToString(null, european) + " " + String.Format(european, "{0:N2}", price));

        Boolean readOnly = NumberFormatInfo.CurrentInfo.IsReadOnly, invariant = NumberFormatInfo.InvariantInfo.CurrencySymbol == "¤";
        String setCaught = "no";
        try { NumberFormatInfo.CurrentInfo.NegativeSign = "x"; }
        catch (InvalidOperationException) { setCaught = "caught"; }

        String nullCaught = "no";
        try { european.PercentSymbol = null; }
        catch (ArgumentException) { nullCaught = "caught"; }

        Console.WriteLine("infos " + readOnly + " " + invariant + " " + european.IsReadOnly + " "
            + Object.ReferenceEquals(NumberFormatInfo.GetInstance(null), NumberFormatInfo.CurrentInfo)
            + " " + Object.ReferenceEquals(NumberFormatInfo.GetInstance(european), european) + " " + setCaught + " " + nullCaught);

        Console.WriteLine("{0}-{1}", 1, 2);
        Console.Write("{0}", "");

        Console.WriteLine("literals " + 255.ToString("x") + " " + 3.5.ToString("F1") + " " + 0xFF.ToString() + " " + 5L.ToString() + " "
            + 3.14159f.ToString("F2") + " " + 2.5f.ToString("F0"));

        Console.WriteLine("caught " + Try("0,", 1234.0) + " " + Try("0.0E+00", 1.0) + " " + Try("'abc", 1.0) + " " + Try("F100", 1.0) + " " + Try("Q", 1.0));

        Single pi = 3.14159f, step = 1.25f;
        Console.WriteLine("single " + pi.ToString("F2") + " " + step.ToString("F1"));
    }
}
