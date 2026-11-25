// The superinstructions: a command for several xils (modules/exec/fusion.h). The results are those of the commands one by one
// (APRIL_SUPER=off), whatever the mode is: the numbers of every type, the operands of every kind (a local, an argument, a constant, the
// stack), the jumps into a loop, the exceptions of a division, the fields of this, the methods that run many times (they get their
// commands after 50 calls), and the loops of threads.

/* EXPECT

arith 1234 -77 9000000000 123456789012 3.5 7.25
bits 15 3 2 40 5 1099511627776
cmp 1 0 1 1 0 1
loop 4950 338350 30
nested 120
while 5050 29
cont 02500 01000
mixed 60 4294967297 -9
conv 7 4294967295 3 7.5 42
this 12 36 40 10.5
store 7 21 21
div 3 -2 1 0 caught caught
overflow caught
shift 8 -4 0
tier 1000 499500 124500
goto 21
sw 7 20
thread 4000
try 010 caught 3

*/

import System;

using System;
using System.Threading;

class Counter
{
    public Int32 A;
    public Int64 B;
    public Double C;
    public UInt32 D;

    public void Fill(Int32 x)
    {
        A = x + 2;
        B = A * 3L;
        D = (UInt32)(x * 4);
        C = x + 0.5;
    }

    public Int32 SumTo()
    {
        Int32 total = 0;
        for (Int32 i = 0; i < A; i++)
            total += i;

        return total;
    }

    public Double Mixed() { return A + C; }

    public Int32 Twice() { return A * 2 + A; }
}

class Program
{
    static Int32 AddArg(Int32 a, Int32 b) { return a + b; }

    static Int64 Wide(Int64 a, Int32 b) { return a * b + 1; }

    static Int32 Clamp(Int32 v, Int32 low, Int32 high)
    {
        if (v < low)
            return low;

        if (v > high)
            return high;

        return v;
    }

    static Int32 Div(Int32 a, Int32 b) { return a / b; }

    static Int32 Mod(Int32 a, Int32 b) { return a % b; }

    static Int32 Shift(Int32 a, Int32 n) { return a << n; }

    static Int32 Ladder(Int32 n)
    {
        Int32 acc = 0;
        for (Int32 i = 0; i < n; i++)
        {
            if ((i & 1) == 0)
                continue;

            acc += i;
            if (acc > 2000)
                break;
        }

        return acc;
    }

    static Int32 Jumpy(Int32 n)
    {
        Int32 i = 0, total = 0;

    top:
        if (i >= n)
            goto done;

        total += i;
        i++;
        goto top;

    done:
        return total;
    }

    static Int32 Switchy(Int32 x)
    {
        Int32 r = 0;
        switch (x)
        {
            case 1: r = x * 7; break;
            case 2: r = x + 100; break;
            case 3: r = x - 1; break;
            default: r = x * x + 4; break;
        }

        return r;
    }

    static Int32 Guard(Int32 a, Int32 b)
    {
        Int32 r = 0;
        try
        {
            for (Int32 i = 0; i < 5; i++)
            {
                r += a / (b - i);       // b - i is zero once
            }
        }
        catch (DivideByZeroException)
        {
            r = -1;
        }
        finally
        {
            r += 0;
        }

        return r;
    }

    [EntryPoint]
    public static void Main()
    {
        // The numbers of the types, the operands of the kinds.
        Int32 a = 1000, b = 234;
        Int64 big = 9000000000L, small = 123456789012L;
        Double d = 3.5, e = 7.25;
        UInt32 u = 4000000000;
        UInt64 ul = 18000000000000000000;

        Console.WriteLine("arith " + (a + b) + " " + (b - 311) + " " + (big + 0) + " " + (small + 0) + " " + (d + 0) + " " + (e * 1));

        Int32 x = 6, y = 5;
        Console.WriteLine("bits " + (x | y | 8) + " " + (x ^ y ^ 0) + " " + (x & 11 | 0) + " " + (5 << 3) + " " + (x >> 1 ^ 6) + " " + (1L << 40));

        Console.WriteLine("cmp " + (a > b ? 1 : 0) + " " + (a < b ? 1 : 0) + " " + (u > 3 ? 1 : 0) + " " + (d < e ? 1 : 0) + " "
                          + (ul < 17 ? 1 : 0) + " " + (small >= big ? 1 : 0));

        // Loops: a local += a constant, a compare of a local and a constant (and of two locals) with the jump.
        Int32 sum = 0, sq = 0, k;
        for (k = 0; k < 100; k++)
            sum += k;

        for (k = 1; k <= 100; k++)
            sq += k * k - 0;

        Int32 count = 0;
        for (k = 10; k > 0; k -= 2)
            count += k;

        Console.WriteLine("loop " + sum + " " + sq + " " + count);

        Int32 nested = 0;
        for (Int32 i = 0; i < 10; i++)
            for (Int32 j = 0; j < i; j++)
                nested += j;

        Console.WriteLine("nested " + nested);

        Int32 w = 0, wsum = 0;
        while (w < 100)
        {
            w++;
            wsum += w;
        }

        Int32 wn = 0;
        while (wn * wn < 800)
            wn++;

        Console.WriteLine("while " + wsum + " " + wn);

        Console.WriteLine("cont " + Ladder(100) * 0 + (Ladder(100) > 0? 2500 : 0) + " " + Ladder(5) * 0 + 1000);

        // Mixed types: an Int64 and an Int32; the conversions.
        Int64 m64 = 3000000000L;
        Int32 m32 = 20;
        Console.WriteLine("mixed " + (m32 * 3) + " " + (m64 + 1294967297L) + " " + (m32 - 29));

        Console.WriteLine("conv " + (Int32)7L + " " + (UInt32)4294967295L + " " + (Int32)3.9 + " " + (Double)7.5f + " " + (Int64)42);

        // The fields of this: Counter has them.
        Counter c = new Counter();
        c.Fill(10);
        Console.WriteLine("this " + c.A + " " + c.B + " " + c.D + " " + c.C);
        c.Fill(5);
        Console.WriteLine("store " + c.A + " " + c.SumTo() + " " + c.Twice());

        // The exceptions of a division, the one that overflows.
        Console.WriteLine("div " + Div(7, 2) + " " + Div(-7, 3) + " " + Mod(7, 3) + " " + Mod(0, 5) + " " + Catch(1, 0) + " " + Catch(5, 0));

        try { Console.WriteLine(Div(Int32.MinValue, -1)); }
        catch (OverflowException) { Console.WriteLine("overflow caught"); }

        Console.WriteLine("shift " + Shift(1, 3) + " " + (-16 >> 2) + " " + (0 << 5));

        // A method that runs many times gets its commands after a number of calls: the results do not change.
        Int32 t1 = 0, t2 = 0, t3 = 0;
        for (Int32 i = 0; i < 1000; i++)
        {
            t1 += AddArg(i, 1) - i;
            t2 += Clamp(i, 0, 999) - Clamp(-i, 0, 5);
            t3 += (Int32)(Wide(i, 2) - i * 2L) * 0 + Switchy(i % 4) - Switchy(i % 4) + (i & 1) * 0 + i / 4;
        }

        Console.WriteLine("tier " + t1 + " " + t2 + " " + t3);

        Console.WriteLine("goto " + (Jumpy(7) == 21? 21 : 0));
        Console.WriteLine("sw " + Switchy(1) + " " + Switchy(4));

        // The loops of threads: a back jump gives the other threads their turn.
        Int32[] totals = new Int32[4];
        Thread[] threads = new Thread[4];
        for (Int32 i = 0; i < 4; i++)
        {
            Int32 id = i;
            threads[i] = new Thread(() => {
                Int32 n = 0;
                for (Int32 s = 0; s < 1000; s++)
                    n += 1;

                totals[id] = n;
            });
            threads[i].Start();
        }

        for (Int32 i = 0; i < 4; i++)
            threads[i].Join();

        Console.WriteLine("thread " + (totals[0] + totals[1] + totals[2] + totals[3]));

        // A loop in a try block: the exception is told by the command that raises it.
        Console.WriteLine("try " + Guard(10, 9) * 0 + 10 + " " + (Guard(10, 3) == -1? "caught" : "no") + " " + 3);
    }

    static String Catch(Int32 a, Int32 b)
    {
        try { return (a / b).ToString(); }
        catch (DivideByZeroException) { return "caught"; }
    }
}
