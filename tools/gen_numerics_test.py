#! /usr/bin/python

# Makes test/unittest/interface/generic_math.cs: the integer types are used through the interfaces of the numbers
# (generic methods that have IBinaryInteger<T> as a constraint), and the results are compared with those of a model of
# the integers (Python numbers that wrap to the width of the type).
#
#       python tools/gen_numerics_test.py

import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from gen_numerics import TYPES, ROOT

OUT = os.path.join(ROOT, 'test', 'unittest', 'interface', 'generic_math.cs')


def model(name, bits, signed):
    mask = (1 << bits) - 1
    lo = -(1 << (bits - 1)) if signed else 0
    hi = (1 << (bits - 1)) - 1 if signed else mask

    def wrap(x):
        x &= mask
        if signed and x >= (1 << (bits - 1)):
            x -= 1 << bits
        return x

    def pattern(x):
        return x & mask

    def lzc(x):
        return bits - pattern(x).bit_length()

    def popcount(x):
        return bin(pattern(x)).count('1')

    def tzc(x):
        p = pattern(x)
        if p == 0:
            return bits
        n = 0
        while (p >> n) & 1 == 0:
            n += 1
        return n

    def rotl(x, n):
        n &= bits - 1
        p = pattern(x)
        return wrap(((p << n) | (p >> (bits - n))) & mask) if n else x

    def rotr(x, n):
        n &= bits - 1
        p = pattern(x)
        return wrap(((p >> n) | (p << (bits - n))) & mask) if n else x

    def div(a, b):
        q = abs(a) // abs(b)
        return wrap(-q if (a < 0) != (b < 0) else q)

    def rem(a, b):
        return wrap(a - div(a, b) * b)

    def shortest(x):
        if signed and x < 0:
            return bits + 1 - lzc(~x)
        return bits - lzc(x)

    def maxmag(x, y):
        if signed:
            ax, ay = x, y
            if x < 0:
                ax = wrap(-x)
                if ax < 0:
                    return x
            if y < 0:
                ay = wrap(-y)
                if ay < 0:
                    return y
            if ax > ay:
                return x
            if ax == ay:
                return y if x < 0 else x
            return y
        return x if x > y else y

    def minmag(x, y):
        if signed:
            ax, ay = x, y
            if x < 0:
                ax = wrap(-x)
                if ax < 0:
                    return y
            if y < 0:
                ay = wrap(-y)
                if ay < 0:
                    return x
            if ax < ay:
                return x
            if ax == ay:
                return x if x < 0 else y
            return y
        return x if x < y else y

    def ops(a, b):
        r = []
        r.append('a=%d b=%d' % (a, b))
        r.append('add=%d' % wrap(a + b))
        r.append('sub=%d' % wrap(a - b))
        r.append('mul=%d' % wrap(a * b))
        r.append('div=%d' % div(a, b))
        r.append('rem=%d' % rem(a, b))
        r.append('neg=%d' % wrap(-a))
        r.append('and=%d' % wrap(a & b))
        r.append('or=%d' % wrap(a | b))
        r.append('xor=%d' % wrap(a ^ b))
        r.append('not=%d' % wrap(~a))
        r.append('shl=%d' % wrap(a << 3))
        r.append('shr=%d' % wrap(a >> 2))
        r.append('inc=%d' % wrap(a + 1))
        r.append('dec=%d' % wrap(a - 1))
        r.append('lt=%s' % ('True' if a < b else 'False'))
        r.append('ge=%s' % ('True' if a >= b else 'False'))
        r.append('eq=%s' % ('True' if a == b else 'False'))
        r.append('ne=%s' % ('True' if a != b else 'False'))
        if signed and a == lo:
            r.append('abs=ovf')
        else:
            r.append('abs=%d' % abs(a))
        r.append('sign=%d' % ((a > 0) - (a < 0)))
        r.append('neg?=%s' % ('True' if a < 0 else 'False'))
        r.append('pos?=%s' % ('True' if a >= 0 else 'False'))
        r.append('even=%s' % ('True' if a % 2 == 0 else 'False'))
        r.append('odd=%s' % ('True' if a % 2 != 0 else 'False'))
        r.append('zero=%s' % ('True' if a == 0 else 'False'))
        r.append('pow2=%s' % ('True' if a > 0 and (a & (a - 1)) == 0 else 'False'))
        if a < 0:
            r.append('log2=ovf')
        else:
            r.append('log2=%d' % (max(a.bit_length() - 1, 0)))
        r.append('lzc=%d' % lzc(a))
        r.append('pop=%d' % popcount(a))
        r.append('tzc=%d' % tzc(a))
        r.append('rotl=%d' % rotl(a, 3))
        r.append('rotr=%d' % rotr(a, 3))
        r.append('max=%d' % max(a, b))
        r.append('min=%d' % min(a, b))
        lo2, hi2 = min(0, b), max(0, b)
        r.append('clamp=%d' % min(max(a, lo2), hi2))
        if signed and ((a < 0) != (b < 0)) and wrap(-a) == a and a != 0:
            r.append('copysign=ovf')
        elif signed and (a < 0) != (b < 0):
            r.append('copysign=%d' % wrap(-a))
        else:
            r.append('copysign=%d' % a)
        r.append('maxmag=%d' % maxmag(a, b))
        r.append('minmag=%d' % minmag(a, b))
        r.append('divrem=%d,%d' % (div(a, b), rem(a, b)))
        r.append('bytes=%d' % (bits // 8))
        r.append('shortest=%d' % shortest(a))
        return ' '.join(r)

    def pairs():
        if signed:
            return [(hi, 3), (lo, 5), (-37, 11), (37, -11), (lo + 1, -1), (0, 7), (-1, -1)]
        return [(hi, 3), (hi >> 1, 5), (37, 11), (11, 37), (1, hi), (0, 7)]

    return dict(wrap=wrap, ops=ops, pairs=pairs, lo=lo, hi=hi, div=div, rem=rem)


def literal(name, value, lo, hi):
    if value == lo:
        return '%s.MinValue' % name
    if value == hi:
        return '%s.MaxValue' % name
    if value < 0:
        return '(%s)(%d)' % (name, value)
    return '(%s)%d' % (name, value)


SOURCE = r'''// The integer types through the interfaces of the numbers (IBinaryInteger<T>, IMinMaxValue<T>, ISignedNumber<T>,
// IUnsignedNumber<T>): the operators, the constants, and the methods of the numbers and of the bits, with the values
// of the limits. The expected text is made by tools/gen_numerics_test.py (a model of the integers).

/* EXPECT

%(expect)s

*/

import System;

using System;
using System.Numerics;

class Util
{
    public static String Constants<T>() where T : IBinaryInteger<T>, IMinMaxValue<T>, IStringable
    {
        return "min=" + T.MinValue.ToString() + " max=" + T.MaxValue.ToString() + " zero=" + T.Zero.ToString()
            + " one=" + T.One.ToString() + " allbits=" + T.AllBitsSet.ToString() + " radix=" + T.Radix
            + " addid=" + T.AdditiveIdentity.ToString() + " mulid=" + T.MultiplicativeIdentity.ToString();
    }

    public static String NegativeOne<T>() where T : ISignedNumber<T>, IStringable
    {
        return "negone=" + T.NegativeOne.ToString();
    }

    public static String Unsigned<T>() where T : IUnsignedNumber<T>, IStringable
    {
        return "unsigned=" + T.Zero.ToString();
    }

    public static String Ops<T>(T a, T b) where T : IBinaryInteger<T>, IStringable
    {
        String r = "a=" + a.ToString() + " b=" + b.ToString();

        r += " add=" + (a + b).ToString();
        r += " sub=" + (a - b).ToString();
        T t;
        t = a * b;
        r += " mul=" + t.ToString();
        t = a / b;
        r += " div=" + t.ToString();
        t = a %% b;
        r += " rem=" + t.ToString();
        r += " neg=" + (-a).ToString();
        t = a & b;
        r += " and=" + t.ToString();
        t = a | b;
        r += " or=" + t.ToString();
        t = a ^ b;
        r += " xor=" + t.ToString();
        t = ~a;
        r += " not=" + t.ToString();
        t = a << 3;
        r += " shl=" + t.ToString();
        t = a >> 2;
        r += " shr=" + t.ToString();

        t = a;
        t++;
        r += " inc=" + t.ToString();
        t = a;
        t--;
        r += " dec=" + t.ToString();

        r += " lt=" + (a < b) + " ge=" + (a >= b) + " eq=" + (a == b) + " ne=" + (a != b);

        try
        {
            t = T.Abs(a);
            r += " abs=" + t.ToString();
        }
        catch (OverflowException)
        {
            r += " abs=ovf";
        }

        r += " sign=" + T.Sign(a) + " neg?=" + T.IsNegative(a) + " pos?=" + T.IsPositive(a) + " even=" + T.IsEvenInteger(a)
            + " odd=" + T.IsOddInteger(a) + " zero=" + T.IsZero(a) + " pow2=" + T.IsPow2(a);

        try
        {
            t = T.Log2(a);
            r += " log2=" + t.ToString();
        }
        catch (OverflowException)
        {
            r += " log2=ovf";
        }

        r += " lzc=" + T.LeadingZeroCount(a).ToString();
        t = T.PopCount(a);
        r += " pop=" + t.ToString();
        t = T.TrailingZeroCount(a);
        r += " tzc=" + t.ToString();
        t = T.RotateLeft(a, 3);
        r += " rotl=" + t.ToString();
        t = T.RotateRight(a, 3);
        r += " rotr=" + t.ToString();

        t = T.Max(a, b);
        r += " max=" + t.ToString();
        t = T.Min(a, b);
        r += " min=" + t.ToString();

        T lo = T.Min(T.Zero, b);
        T hi = T.Max(T.Zero, b);
        t = T.Clamp(a, lo, hi);
        r += " clamp=" + t.ToString();

        try
        {
            t = T.CopySign(a, b);
            r += " copysign=" + t.ToString();
        }
        catch (OverflowException)
        {
            r += " copysign=ovf";
        }

        t = T.MaxMagnitude(a, b);
        r += " maxmag=" + t.ToString();
        t = T.MinMagnitude(a, b);
        r += " minmag=" + t.ToString();

        Tuple<T, T> dr = T.DivRem(a, b);
        T quotient = dr.Item1;
        T remainder = dr.Item2;
        r += " divrem=" + quotient.ToString() + "," + remainder.ToString();

        r += " bytes=" + a.GetByteCount() + " shortest=" + a.GetShortestBitLength();
        return r;
    }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
%(main)s
    }
}
'''


def main():
    expect = []
    main_lines = []

    for name, bits, signed, unsigned, minimum, maximum in TYPES:
        m = model(name, bits, signed)
        lo, hi = m['lo'], m['hi']

        const = 'min=%d max=%d zero=0 one=1 allbits=%d radix=2 addid=0 mulid=1' % (lo, hi, -1 if signed else hi)
        expect.append('%s %s' % (name, const))
        main_lines.append('        Console.WriteLine("%s " + Util.Constants<%s>());' % (name, name))

        if signed:
            expect.append('%s negone=-1' % name)
            main_lines.append('        Console.WriteLine("%s " + Util.NegativeOne<%s>());' % (name, name))
        else:
            expect.append('%s unsigned=0' % name)
            main_lines.append('        Console.WriteLine("%s " + Util.Unsigned<%s>());' % (name, name))

        for a, b in m['pairs']():
            expect.append('%s %s' % (name, m['ops'](a, b)))
            main_lines.append('        Console.WriteLine("%s " + Util.Ops(%s, %s));'
                              % (name, literal(name, a, lo, hi), literal(name, b, lo, hi)))

        # DivRem of the type itself.
        a, b = (hi, 3)
        q, r = m['div'](a, b), m['rem'](a, b)
        expect.append('%s divrem %d %d' % (name, q, r))
        main_lines.append('        Tuple<%s, %s> %s_dr = %s.DivRem(%s, %s);'
                          % (name, name, name, name, literal(name, a, lo, hi), literal(name, b, lo, hi)))
        main_lines.append('        Console.WriteLine("%s divrem " + %s_dr.Item1 + " " + %s_dr.Item2);' % (name, name, name))

    text = SOURCE % dict(expect='\n'.join(expect), main='\n'.join(main_lines))
    open(OUT, 'w', encoding='utf-8', newline='').write(text)
    print('written', OUT, len(expect), 'lines')

    convert = convert_main()
    open(OUT_CONVERT, 'w', encoding='utf-8', newline='').write(convert)
    print('written', OUT_CONVERT)

    checked_text = checked_main()
    open(OUT_CHECKED, 'w', encoding='utf-8', newline='').write(checked_text)
    print('written', OUT_CHECKED)


OUT_CONVERT = os.path.join(ROOT, 'test', 'unittest', 'interface', 'generic_math_convert.cs')

CONVERT_SOURCE = r"""// The conversions between the integer types through INumberBase<T>: CreateChecked (OverflowException when the value
// does not fit), CreateSaturating (the lowest or the highest value), CreateTruncating (the low bits), for each pair of
// the 8 types. The expected text is made by tools/gen_numerics_test.py (a model of the integers).

/* EXPECT

%(expect)s

*/

import System;

using System;
using System.Numerics;

class Util
{
    public static String Row<T, U>(U[] values) where T : INumberBase<T>, IStringable where U : INumberBase<U>, IStringable
    {
        String r = "";

        for (Int32 k = 0; k < values.Length; k++)
        {
            U u = values[k];
            r += (k > 0? " " : "") + u.ToString() + ":";

            try
            {
                T c = T.CreateChecked(u);
                r += c.ToString();
            }
            catch (OverflowException)
            {
                r += "ovf";
            }

            T s = T.CreateSaturating(u);
            T t = T.CreateTruncating(u);
            r += "/" + s.ToString() + "/" + t.ToString();
        }

        return r;
    }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
%(main)s
    }
}
"""

CONVERT_VALUES = [0, 1, -1, 100, -100, 127, 128, 255, 256, 32767, 32768, 65535, 65536, 2147483647, 2147483648,
                  4294967295, 4294967296, 9223372036854775807]


def convert_main():
    expect = []
    main_lines = []

    for tname, tbits, tsigned, _, _, _ in TYPES:
        t = model(tname, tbits, tsigned)
        mask = (1 << tbits) - 1

        for sname, sbits, ssigned, _, _, _ in TYPES:
            slo = -(1 << (sbits - 1)) if ssigned else 0
            shi = (1 << (sbits - 1)) - 1 if ssigned else (1 << sbits) - 1

            values = [slo, shi]
            for v in CONVERT_VALUES:
                if slo <= v <= shi and v not in values:
                    values.append(v)

            values.sort()

            row = []
            for v in values:
                if t['lo'] <= v <= t['hi']:
                    checked = '%d' % v
                else:
                    checked = 'ovf'

                saturated = min(max(v, t['lo']), t['hi'])
                truncated = t['wrap'](v)
                row.append('%d:%s/%d/%d' % (v, checked, saturated, truncated))

            expect.append('%s<-%s %s' % (tname, sname, ' '.join(row)))

            items = ', '.join(literal(sname, v, slo, shi) for v in values)
            main_lines.append('        Console.WriteLine("%s<-%s " + Util.Row<%s, %s>(new %s[] { %s }));'
                              % (tname, sname, tname, sname, sname, items))

    return CONVERT_SOURCE % dict(expect='\n'.join(expect), main='\n'.join(main_lines))


OUT_CHECKED = os.path.join(ROOT, 'test', 'unittest', 'interface', 'generic_math_checked.cs')

CHECKED_SOURCE = r"""// The operators of the integer types in a checked context, through the interfaces of the numbers (IBinaryInteger<T>):
// checked(a + b), checked(-a) and a checked block for ++ and --, raise OverflowException when the result does not fit
// the type. The expected text is made by tools/gen_numerics_test.py (a model of the integers).

/* EXPECT

%(expect)s

*/

import System;

using System;
using System.Numerics;

class Util
{
    public static String Checked<T>(T a, T b) where T : IBinaryInteger<T>, IStringable
    {
        String r = "a=" + a.ToString() + " b=" + b.ToString();

        try { T t = checked(a + b); r += " add=" + t.ToString(); } catch (OverflowException) { r += " add=ovf"; }
        try { T t = checked(a - b); r += " sub=" + t.ToString(); } catch (OverflowException) { r += " sub=ovf"; }
        try { T t = checked(a * b); r += " mul=" + t.ToString(); } catch (OverflowException) { r += " mul=ovf"; }
        try { T t = checked(a / b); r += " div=" + t.ToString(); } catch (OverflowException) { r += " div=ovf"; }
        try { T t = checked(-a); r += " neg=" + t.ToString(); } catch (OverflowException) { r += " neg=ovf"; }

        try
        {
            T t = a;
            checked { t++; }
            r += " inc=" + t.ToString();
        }
        catch (OverflowException)
        {
            r += " inc=ovf";
        }

        try
        {
            T t = a;
            checked { t--; }
            r += " dec=" + t.ToString();
        }
        catch (OverflowException)
        {
            r += " dec=ovf";
        }

        // Not checked: the same operators wrap.
        T w = a + b;
        r += " wrap=" + w.ToString();
        return r;
    }
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
%(main)s
    }
}
"""


def checked_main():
    expect = []
    main_lines = []

    for name, bits, signed, _, _, _ in TYPES:
        m = model(name, bits, signed)
        lo, hi = m['lo'], m['hi']
        wrap = m['wrap']

        if signed:
            pairs = [(hi, 3), (lo, 5), (-37, 11), (37, -11), (lo, -1), (hi, hi), (lo + 1, -1), (0, 7), (hi, 1), (lo, 1)]
        else:
            pairs = [(hi, 3), (hi >> 1, 5), (37, 11), (11, 37), (1, hi), (hi, 1), (0, 7), (hi, hi)]

        def cell(v):
            return '%d' % v if lo <= v <= hi else 'ovf'

        for a, b in pairs:
            q = abs(a) // abs(b)
            q = -q if (a < 0) != (b < 0) else q

            parts = ['a=%d b=%d' % (a, b),
                     'add=%s' % cell(a + b), 'sub=%s' % cell(a - b), 'mul=%s' % cell(a * b), 'div=%s' % cell(q),
                     'neg=%s' % cell(-a), 'inc=%s' % cell(a + 1), 'dec=%s' % cell(a - 1),
                     'wrap=%d' % wrap(a + b)]
            expect.append('%s %s' % (name, ' '.join(parts)))
            main_lines.append('        Console.WriteLine("%s " + Util.Checked(%s, %s));'
                              % (name, literal(name, a, lo, hi), literal(name, b, lo, hi)))

    return CHECKED_SOURCE % dict(expect='\n'.join(expect), main='\n'.join(main_lines))


if __name__ == '__main__':
    main()
