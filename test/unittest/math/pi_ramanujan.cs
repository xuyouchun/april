// The first 1000 decimal digits of pi, by the series of Ramanujan:
//
//   1 / pi = (2 * sqrt(2) / 9801) * sum(k >= 0) (4k)! (1103 + 26390 k) / ((k!)^4 * 396^(4k))
//
// Each term gives about 8 digits, so 135 terms are more than enough. The numbers are fixed point
// numbers with limbs of base 10000 (an array of Int64, the first limbs are the fraction), so the
// program does not need a library of big numbers: the terms are made one from the other with small
// multiplications and divisions, the square root of 2 and the division at the end are made by the
// method of Newton, with the multiplication of two numbers.

/* EXPECT

3.
14159265358979323846264338327950288419716939937510
58209749445923078164062862089986280348253421170679
82148086513282306647093844609550582231725359408128
48111745028410270193852110555964462294895493038196
44288109756659334461284756482337867831652712019091
45648566923460348610454326648213393607260249141273
72458700660631558817488152092096282925409171536436
78925903600113305305488204665213841469519415116094
33057270365759591953092186117381932611793105118548
07446237996274956735188575272489122793818301194912
98336733624406566430860213949463952247371907021798
60943702770539217176293176752384674818467669405132
00056812714526356082778577134275778960917363717872
14684409012249534301465495853710507922796892589235
42019956112129021960864034418159813629774771309960
51870721134999999837297804995105973173281609631859
50244594553469083026425223082533446850352619311881
71010003137838752886587533208381420617177669147303
59825349042875546873115956286388235378759375195778
18577805321712268066130019278766111959092164201989
done

*/

import System;

using System;

class Pi
{
    // The base of the limbs, and the count of the limbs of the fraction: 4 digits for a limb,
    // 260 limbs are 1040 digits, 40 digits more than the digits that are printed.
    private const Int64 Base = 10000;
    private const Int32 Frac = 260;
    private const Int32 Size = Frac + 2;

    // The number of an integer, that is less than Base * Base.
    private static Int64[] Make(Int64 value)
    {
        Int64[] a = new Int64[Size];
        a[Frac] = value % Base;
        a[Frac + 1] = value / Base;
        return a;
    }

    // The number of a Double, it is about 15 digits.
    private static Int64[] FromDouble(Double value)
    {
        Int64[] a = new Int64[Size];

        Int64 whole = (Int64)value;
        a[Frac] = whole % Base;
        a[Frac + 1] = whole / Base;

        Double fraction = value - (Double)whole;
        for (Int32 k = Frac - 1; k >= Frac - 5; k--)
        {
            fraction = fraction * 10000.0;
            Int64 limb = (Int64)fraction;
            a[k] = limb;
            fraction = fraction - (Double)limb;
        }

        return a;
    }

    private static Double ToDouble(Int64[] a)
    {
        Double value = 0.0;
        for (Int32 k = Size - 1; k >= Frac - 5; k--)
            value = value * 10000.0 + (Double)(a[k]);

        for (Int32 k = 0; k < 5; k++)
            value = value / 10000.0;

        return value;
    }

    private static Int64[] Add(Int64[] a, Int64[] b)
    {
        Int64[] r = new Int64[Size];
        Int64 carry = 0;

        for (Int32 k = 0; k < Size; k++)
        {
            Int64 v = a[k] + b[k] + carry;
            r[k] = v % Base;
            carry = v / Base;
        }

        return r;
    }

    // a - b, a is not less than b.
    private static Int64[] Sub(Int64[] a, Int64[] b)
    {
        Int64[] r = new Int64[Size];
        Int64 borrow = 0;

        for (Int32 k = 0; k < Size; k++)
        {
            Int64 v = a[k] - b[k] - borrow;
            if (v < 0)
            {
                v = v + Base;
                borrow = 1;
            }
            else
            {
                borrow = 0;
            }

            r[k] = v;
        }

        return r;
    }

    private static Int64[] MulSmall(Int64[] a, Int64 m)
    {
        Int64[] r = new Int64[Size];
        Int64 carry = 0;

        for (Int32 k = 0; k < Size; k++)
        {
            Int64 v = a[k] * m + carry;
            r[k] = v % Base;
            carry = v / Base;
        }

        return r;
    }

    private static Int64[] DivSmall(Int64[] a, Int64 d)
    {
        Int64[] r = new Int64[Size];
        Int64 rest = 0;

        for (Int32 k = Size - 1; k >= 0; k--)
        {
            Int64 v = rest * Base + a[k];
            r[k] = v / d;
            rest = v % d;
        }

        return r;
    }

    // a * b: the product has twice the limbs of the fraction, the lower ones are cut.
    private static Int64[] Mul(Int64[] a, Int64[] b)
    {
        Int64[] wide = new Int64[Size * 2];

        for (Int32 i = 0; i < Size; i++)
        {
            if (a[i] == 0)
                continue;

            for (Int32 j = 0; j < Size; j++)
                wide[i + j] = wide[i + j] + a[i] * b[j];
        }

        Int64 carry = 0;
        for (Int32 k = 0; k < Size * 2; k++)
        {
            Int64 v = wide[k] + carry;
            wide[k] = v % Base;
            carry = v / Base;
        }

        Int64[] r = new Int64[Size];
        for (Int32 k = 0; k < Size; k++)
            r[k] = wide[k + Frac];

        return r;
    }

    // The sum of the series: sum of (4k)! (1103 + 26390 k) / ((k!)^4 396^(4k)), each term is made
    // from the one before.
    private static Int64[] Series(Int32 terms)
    {
        Int64[] term = Make(1);
        Int64[] sum = Make(1103);

        for (Int32 k = 1; k <= terms; k++)
        {
            Int64 n = 4 * k;
            term = MulSmall(term, n);
            term = MulSmall(term, n - 1);
            term = MulSmall(term, n - 2);
            term = MulSmall(term, n - 3);

            for (Int32 i = 0; i < 4; i++)
            {
                term = DivSmall(term, k);
                term = DivSmall(term, 396);
            }

            sum = Add(sum, MulSmall(term, 1103 + 26390 * k));
        }

        return sum;
    }

    // 1 / sqrt(2), by the iteration y = y * (3 - 2 y y) / 2: the digits are twice at each step.
    private static Int64[] InverseSqrt2()
    {
        Int64[] y = FromDouble(0.70710678118654752440);

        for (Int32 step = 0; step < 10; step++)
        {
            Int64[] y2 = MulSmall(Mul(y, y), 2);
            y = DivSmall(Mul(y, Sub(Make(3), y2)), 2);
        }

        return y;
    }

    // 1 / x, by the iteration y = y * (2 - x y).
    private static Int64[] Reciprocal(Int64[] x)
    {
        Int64[] y = FromDouble(1.0 / ToDouble(x));

        for (Int32 step = 0; step < 10; step++)
            y = Mul(y, Sub(Make(2), Mul(x, y)));

        return y;
    }

    // The digits of the fraction as text, 4 for a limb.
    private static String Digits(Int64[] a)
    {
        String text = "";

        for (Int32 k = Frac - 1; k >= 0; k--)
        {
            String limb = "" + a[k];
            while (limb.Length < 4)
                limb = "0" + limb;

            text = text + limb;
        }

        return text;
    }

    [EntryPoint]
    public static void Main()
    {
        Int64[] sum = Series(135);

        // pi = 9801 / (2 sqrt(2) sum) = 9801 * (1 / (2 * 2 * (1 / sqrt(2)) * sum)).
        Int64[] divisor = MulSmall(Mul(InverseSqrt2(), sum), 4);
        Int64[] pi = MulSmall(Reciprocal(divisor), 9801);

        // 1 / sqrt(2) * 4 is 2 sqrt(2): the divisor is 2 sqrt(2) * sum.
        Console.WriteLine(pi[Frac] + ".");

        String digits = Digits(pi);
        for (Int32 line = 0; line < 20; line++)
            Console.WriteLine(digits.Substring(line * 50, 50));

        Console.WriteLine("done");
    }
}
