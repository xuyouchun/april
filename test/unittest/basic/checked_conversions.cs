// checked: the conversions of numbers raise OverflowException when the value does not fit the type.

/* EXPECT

-1294967296
4294967295
long->int big overflow
long->int small=100
long->uint neg overflow
int->uint minus overflow
uint->int overflow
double->int big overflow
double->int ok=-2
float->int overflow
long->byte=100
long->byte big overflow
long->short overflow
int->long=-1
unchecked=-1294967296
char overflow
char ok=100
ulong overflow
long->ulong ok=100
neg double->uint overflow
trunc -0.9=0
nan overflow
4000000000
3000000000

*/

import System;

using System;

class Program
{
    [EntryPoint]
    public static void Main()
    {
        long big = 3000000000;
        long small = 100;
        long neg = -5;
        int minus = -1;
        uint u = (uint)4000000000;
        double d = 1e20;
        double ok = -2.9;
        float f = 3e9f;

        // Not checked: wraps.
        Console.WriteLine((int)big);
        Console.WriteLine((uint)minus);

        try
        {
            checked
            {
                Console.WriteLine("long->int big=" + (int)big);
            }
        }
        catch (OverflowException)
        {
            Console.WriteLine("long->int big overflow");
        }

        try
        {
            checked
            {
                Console.WriteLine("long->int small=" + (int)small);
            }
        }
        catch (OverflowException)
        {
            Console.WriteLine("long->int small overflow");
        }

        try
        {
            checked
            {
                Console.WriteLine("long->uint neg=" + (int)(uint)neg);
            }
        }
        catch (OverflowException)
        {
            Console.WriteLine("long->uint neg overflow");
        }

        try
        {
            checked
            {
                Console.WriteLine("int->uint minus=" + (int)(uint)minus);
            }
        }
        catch (OverflowException)
        {
            Console.WriteLine("int->uint minus overflow");
        }

        try
        {
            checked
            {
                Console.WriteLine("uint->int=" + (int)u);
            }
        }
        catch (OverflowException)
        {
            Console.WriteLine("uint->int overflow");
        }

        try
        {
            checked
            {
                Console.WriteLine("double->int big=" + (int)d);
            }
        }
        catch (OverflowException)
        {
            Console.WriteLine("double->int big overflow");
        }

        try
        {
            checked
            {
                Console.WriteLine("double->int ok=" + (int)ok);
            }
        }
        catch (OverflowException)
        {
            Console.WriteLine("double->int ok overflow");
        }

        try
        {
            checked
            {
                Console.WriteLine("float->int=" + (int)f);
            }
        }
        catch (OverflowException)
        {
            Console.WriteLine("float->int overflow");
        }

        try
        {
            checked
            {
                Console.WriteLine("long->byte=" + (int)(byte)small);
            }
        }
        catch (OverflowException)
        {
            Console.WriteLine("long->byte overflow");
        }

        try
        {
            checked
            {
                Console.WriteLine("long->byte big=" + (int)(byte)(small + 200));
            }
        }
        catch (OverflowException)
        {
            Console.WriteLine("long->byte big overflow");
        }

        try
        {
            checked
            {
                Console.WriteLine("long->short=" + (int)(short)(small * 1000));
            }
        }
        catch (OverflowException)
        {
            Console.WriteLine("long->short overflow");
        }

        try
        {
            checked
            {
                Console.WriteLine("int->long=" + (int)(long)minus);
            }
        }
        catch (OverflowException)
        {
            Console.WriteLine("int->long overflow");
        }

        try
        {
            checked
            {
                Console.WriteLine("unchecked=" + unchecked((int)big));
            }
        }
        catch (OverflowException)
        {
            Console.WriteLine("unchecked overflow");
        }

        try
        {
            checked
            {
                Console.WriteLine("char=" + (int)(char)(small + 70000));
            }
        }
        catch (OverflowException)
        {
            Console.WriteLine("char overflow");
        }

        try
        {
            checked
            {
                Console.WriteLine("char ok=" + (int)(char)small);
            }
        }
        catch (OverflowException)
        {
            Console.WriteLine("char ok overflow");
        }

        try
        {
            checked
            {
                Console.WriteLine("ulong=" + (int)(ulong)neg);
            }
        }
        catch (OverflowException)
        {
            Console.WriteLine("ulong overflow");
        }

        try
        {
            checked
            {
                Console.WriteLine("long->ulong ok=" + (int)(ulong)small);
            }
        }
        catch (OverflowException)
        {
            Console.WriteLine("long->ulong ok overflow");
        }

        try
        {
            checked
            {
                Console.WriteLine("neg double->uint=" + (int)(uint)ok);
            }
        }
        catch (OverflowException)
        {
            Console.WriteLine("neg double->uint overflow");
        }

        try
        {
            checked
            {
                Console.WriteLine("trunc -0.9=" + (int)(uint)(ok + 2.5));
            }
        }
        catch (OverflowException)
        {
            Console.WriteLine("trunc -0.9 overflow");
        }

        double nan = d - d;
        nan = nan / nan;
        try { Console.WriteLine(checked((int)nan)); }
        catch (OverflowException) { Console.WriteLine("nan overflow"); }

        Console.WriteLine(checked((long)u));
        Console.WriteLine(checked((double)big));
    }
}
