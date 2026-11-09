#! /usr/bin/python

# Makes the members of the generic math (the operators, the constants, the numeric methods) of the integer types, writes
# them into core_lib/System/<Type>.cs between the two marks, and sets the interfaces of the types in +types.cs.
# The members are the same for the types, they differ in the width, in the sign and in the limits. Run it again after
# changing a template here: the text between the marks is replaced, what is outside of it is not touched.
#
#       python tools/gen_numerics.py

import os
import re

ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..')
LIB = os.path.join(ROOT, 'core_lib', 'System')

BEGIN = '    // ---- generic math: made by tools/gen_numerics.py, do not edit it here ----'
END = '    // ---- end of generic math ----'

# name, bits, signed, unsigned type of the same width, minimum, maximum
TYPES = [
    ('Int8',   8,  True,  'UInt8',  '(Int8)(-128)',            '(Int8)127'),
    ('UInt8',  8,  False, 'UInt8',  '(UInt8)0',                '(UInt8)255'),
    ('Int16',  16, True,  'UInt16', '(Int16)(-32768)',         '(Int16)32767'),
    ('UInt16', 16, False, 'UInt16', '(UInt16)0',               '(UInt16)65535'),
    ('Int32',  32, True,  'UInt32', '-2147483647 - 1',        '2147483647'),
    ('UInt32', 32, False, 'UInt32', '(UInt32)0',               '(UInt32)4294967295'),
    ('Int64',  64, True,  'UInt64', '-9223372036854775807 - 1', '9223372036854775807'),
    ('UInt64', 64, False, 'UInt64', '(UInt64)0',               '(UInt64)18446744073709551615'),
]


def members(name, bits, signed, unsigned, minimum, maximum):
    T = name
    narrow = bits < 32

    # The value of the type as the number that the operators of the language take: a type that is
    # narrower than 32 bits is an Int32 there, the others are themselves.
    def w(x):
        return '(Int32)%s' % x if narrow else x

    # The unsigned number of the width of the bits (the type of the bits, in the methods of the bits).
    UB = 'UInt64' if bits == 64 else 'UInt32'

    zero = '0' if narrow else '(%s)0' % T       # In the expressions of w().
    one = '1' if narrow else '(%s)1' % T
    shift_mask = 63 if bits == 64 else 31
    out = []

    def add(text):
        out.append(text.strip('\n'))

    # --- the operators
    for op, interface in (('+', 'add'), ('-', 'subtract'), ('*', 'multiply'), ('/', 'divide'), ('%', 'modulo')):
        add('''
    public static %(T)s operator %(op)s(%(T)s left, %(T)s right) => (%(T)s)(%(l)s %(op)s %(r)s);''' % dict(T=T, op=op, l=w('left'), r=w('right')))

    add('''
    public static %(T)s operator +(%(T)s value) => value;''' % dict(T=T))

    if narrow or signed:
        add('''
    public static %(T)s operator -(%(T)s value) => (%(T)s)(-%(v)s);''' % dict(T=T, v=w('value')))
    else:
        add('''
    public static %(T)s operator -(%(T)s value) => (%(T)s)(~value + (%(T)s)1);''' % dict(T=T))

    add('''
    public static %(T)s operator ++(%(T)s value) => (%(T)s)(%(v)s + %(one)s);

    public static %(T)s operator --(%(T)s value) => (%(T)s)(%(v)s - %(one)s);''' % dict(T=T, v=w('value'), one=one))

    for op in ('&', '|', '^'):
        add('''
    public static %(T)s operator %(op)s(%(T)s left, %(T)s right) => (%(T)s)(%(l)s %(op)s %(r)s);''' % dict(T=T, op=op, l=w('left'), r=w('right')))

    add('''
    public static %(T)s operator ~(%(T)s value) => (%(T)s)(~%(v)s);

    public static %(T)s operator <<(%(T)s value, Int32 shiftAmount) => (%(T)s)(%(v)s << (shiftAmount & %(m)d));

    public static %(T)s operator >>(%(T)s value, Int32 shiftAmount) => (%(T)s)(%(v)s >> (shiftAmount & %(m)d));''' % dict(T=T, v=w('value'), m=shift_mask))

    for op in ('==', '!=', '<', '>', '<=', '>='):
        add('''
    public static Boolean operator %(op)s(%(T)s left, %(T)s right) => %(l)s %(op)s %(r)s;''' % dict(T=T, op=op, l=w('left'), r=w('right')))

    # --- the operators in a checked context: they raise OverflowException when the result does not fit.
    for op in ('+', '-', '*', '/'):
        add("""
    public static %(T)s operator checked %(op)s(%(T)s left, %(T)s right) => checked((%(T)s)(%(l)s %(op)s %(r)s));""" % dict(T=T, op=op, l=w('left'), r=w('right')))

    # The negation of the unsigned types raises for any value but 0 (the narrow ones and the signed ones are in Int32 / Int64).
    if bits == 32 and not signed:
        negation = '-(Int64)value'
    elif bits == 64 and not signed:
        negation = '(%s)0 - value' % T
    else:
        negation = '-%s' % w('value')

    add("""
    public static %(T)s operator checked -(%(T)s value) => checked((%(T)s)(%(neg)s));

    public static %(T)s operator checked ++(%(T)s value) => checked((%(T)s)(%(v)s + %(one)s));

    public static %(T)s operator checked --(%(T)s value) => checked((%(T)s)(%(v)s - %(one)s));""" % dict(T=T, neg=negation, v=w('value'), one=one))

    # --- the constants
    add('''
    public static %(T)s MinValue => %(min)s;

    public static %(T)s MaxValue => %(max)s;

    public static %(T)s Zero => (%(T)s)0;

    public static %(T)s One => (%(T)s)1;

    public static Int32 Radix => 2;

    public static %(T)s AdditiveIdentity => (%(T)s)0;

    public static %(T)s MultiplicativeIdentity => (%(T)s)1;

    public static %(T)s AllBitsSet => %(all)s;''' % dict(T=T, min=minimum, max=maximum, all=('(%s)(-1)' % T) if signed else maximum))

    if signed:
        add('''
    public static %(T)s NegativeOne => (%(T)s)(-1);''' % dict(T=T))

    # --- the numbers
    if signed:
        add('''
    public static %(T)s Abs(%(T)s value)
    {
        if (%(v)s < %(zero)s)
        {
            %(T)s result = (%(T)s)(-%(v)s);
            if (%(r)s < %(zero)s)
                throw new OverflowException();

            return result;
        }

        return value;
    }''' % dict(T=T, v=w('value'), r=w('result'), zero=zero))
    else:
        add('''
    public static %(T)s Abs(%(T)s value) => value;''' % dict(T=T))

    add('''
    public static Boolean IsCanonical(%(T)s value) => true;

    public static Boolean IsComplexNumber(%(T)s value) => false;

    public static Boolean IsEvenInteger(%(T)s value) => (%(v)s & %(one)s) == %(zero)s;

    public static Boolean IsFinite(%(T)s value) => true;

    public static Boolean IsImaginaryNumber(%(T)s value) => false;

    public static Boolean IsInfinity(%(T)s value) => false;

    public static Boolean IsInteger(%(T)s value) => true;

    public static Boolean IsNaN(%(T)s value) => false;

    public static Boolean IsNegative(%(T)s value) => %(neg)s;

    public static Boolean IsNegativeInfinity(%(T)s value) => false;

    public static Boolean IsNormal(%(T)s value) => %(v)s != %(zero)s;

    public static Boolean IsOddInteger(%(T)s value) => (%(v)s & %(one)s) != %(zero)s;

    public static Boolean IsPositive(%(T)s value) => %(pos)s;

    public static Boolean IsPositiveInfinity(%(T)s value) => false;

    public static Boolean IsRealNumber(%(T)s value) => true;

    public static Boolean IsSubnormal(%(T)s value) => false;

    public static Boolean IsZero(%(T)s value) => %(v)s == %(zero)s;''' % dict(
        T=T, v=w('value'), zero=zero, one=one,
        neg=('%s < %s' % (w('value'), zero)) if signed else 'false',
        pos=('%s >= %s' % (w('value'), zero)) if signed else 'true'))

    if signed:
        add('''
    public static %(T)s MaxMagnitude(%(T)s x, %(T)s y)
    {
        %(T)s ax = x, ay = y;

        if (%(x)s < %(zero)s)
        {
            ax = (%(T)s)(-%(x)s);
            if (%(ax)s < %(zero)s)
                return x;
        }

        if (%(y)s < %(zero)s)
        {
            ay = (%(T)s)(-%(y)s);
            if (%(ay)s < %(zero)s)
                return y;
        }

        if (%(ax)s > %(ay)s)
            return x;

        if (%(ax)s == %(ay)s)
            return %(x)s < %(zero)s? y : x;

        return y;
    }

    public static %(T)s MaxMagnitudeNumber(%(T)s x, %(T)s y) => MaxMagnitude(x, y);

    public static %(T)s MinMagnitude(%(T)s x, %(T)s y)
    {
        %(T)s ax = x, ay = y;

        if (%(x)s < %(zero)s)
        {
            ax = (%(T)s)(-%(x)s);
            if (%(ax)s < %(zero)s)
                return y;
        }

        if (%(y)s < %(zero)s)
        {
            ay = (%(T)s)(-%(y)s);
            if (%(ay)s < %(zero)s)
                return x;
        }

        if (%(ax)s < %(ay)s)
            return x;

        if (%(ax)s == %(ay)s)
            return %(x)s < %(zero)s? x : y;

        return y;
    }

    public static %(T)s MinMagnitudeNumber(%(T)s x, %(T)s y) => MinMagnitude(x, y);''' % dict(
            T=T, zero=zero, x=w('x'), y=w('y'), ax=w('ax'), ay=w('ay')))
    else:
        add('''
    public static %(T)s MaxMagnitude(%(T)s x, %(T)s y) => %(x)s > %(y)s? x : y;

    public static %(T)s MaxMagnitudeNumber(%(T)s x, %(T)s y) => %(x)s > %(y)s? x : y;

    public static %(T)s MinMagnitude(%(T)s x, %(T)s y) => %(x)s < %(y)s? x : y;

    public static %(T)s MinMagnitudeNumber(%(T)s x, %(T)s y) => %(x)s < %(y)s? x : y;''' % dict(
            T=T, x=w('x'), y=w('y')))

    if signed:
        add('''
    public static Int32 Sign(%(T)s value) => %(v)s < %(zero)s? -1 : (%(v)s > %(zero)s? 1 : 0);''' % dict(T=T, v=w('value'), zero=zero))
    else:
        add('''
    public static Int32 Sign(%(T)s value) => %(v)s == %(zero)s? 0 : 1;''' % dict(T=T, v=w('value'), zero=zero))

    # --- the bits
    ubits = '(%s)(%s)value' % (UB, unsigned) if unsigned != UB and bits == 32 or signed else '(%s)value' % UB
    if bits == 32 or bits == 64:
        ubits = '(%s)value' % unsigned
    else:
        ubits = '(%s)(%s)value' % (UB, unsigned) if signed else '(%s)value' % UB

    ub_zero, ub_one = '(%s)0' % UB, '(%s)1' % UB

    add('''
    public static Boolean IsPow2(%(T)s value) => %(pos)s%(v)s != %(zero)s && (%(v)s & (%(v)s - %(one)s)) == %(zero)s;''' % dict(
        T=T, v=w('value'), zero=zero, one=one, pos=('%s > %s && ' % (w('value'), zero)) if signed else ''))

    if signed:
        add('''
    public static %(T)s Log2(%(T)s value)
    {
        if (%(v)s < %(zero)s)
            throw new OverflowException();

        return (%(T)s)(%(bits)d - 1 - (Int32)LeadingZeroCount((%(T)s)(%(v)s | %(one)s)));
    }''' % dict(T=T, v=w('value'), zero=zero, one=one, bits=bits))
    else:
        add('''
    public static %(T)s Log2(%(T)s value) => (%(T)s)(%(bits)d - 1 - (Int32)LeadingZeroCount((%(T)s)(%(v)s | %(one)s)));''' % dict(
            T=T, v=w('value'), one=one, bits=bits))

    add('''
    public static Tuple<%(T)s, %(T)s> DivRem(%(T)s left, %(T)s right) =>
        new Tuple<%(T)s, %(T)s>((%(T)s)(%(l)s / %(r)s), (%(T)s)(%(l)s %% %(r)s));

    public static %(T)s LeadingZeroCount(%(T)s value)
    {
        %(UB)s bits = %(ubits)s;
        Int32 count = 0;

        for (Int32 bit = %(bits)d - 1; bit >= 0; bit--)
        {
            if ((%(UB)s)((%(UB)s)(bits >> bit) & %(one)s) != %(zero)s)
                break;

            count++;
        }

        return (%(T)s)count;
    }

    public static %(T)s PopCount(%(T)s value)
    {
        %(UB)s bits = %(ubits)s;
        Int32 count = 0;

        while (bits != %(zero)s)
        {
            bits = (%(UB)s)(bits & (%(UB)s)(bits - %(one)s));
            count++;
        }

        return (%(T)s)count;
    }

    public static %(T)s TrailingZeroCount(%(T)s value)
    {
        %(UB)s bits = %(ubits)s;
        if (bits == %(zero)s)
            return (%(T)s)%(bits)d;

        Int32 count = 0;
        while ((%(UB)s)((%(UB)s)(bits >> count) & %(one)s) == %(zero)s)
            count++;

        return (%(T)s)count;
    }

    public static %(T)s RotateLeft(%(T)s value, Int32 rotateAmount)
    {
        Int32 n = rotateAmount & %(mask)d;
        if (n == 0)
            return value;

        %(UB)s bits = %(ubits)s;
        return (%(T)s)((%(UB)s)(bits << n) | (%(UB)s)(bits >> (%(bits)d - n)));
    }

    public static %(T)s RotateRight(%(T)s value, Int32 rotateAmount)
    {
        Int32 n = rotateAmount & %(mask)d;
        if (n == 0)
            return value;

        %(UB)s bits = %(ubits)s;
        return (%(T)s)((%(UB)s)(bits >> n) | (%(UB)s)(bits << (%(bits)d - n)));
    }

    public Int32 GetByteCount() => %(bytes)d;''' % dict(
        T=T, UB=UB, ubits=ubits, bits=bits, bytes=bits // 8, mask=bits - 1, zero=ub_zero, one=ub_one,
        l=w('left'), r=w('right')))

    if signed:
        add('''
    public Int32 GetShortestBitLength()
    {
        %(T)s value = this;

        if (%(v)s >= %(zero)s)
            return %(bits)d - (Int32)LeadingZeroCount(value);

        return %(bits1)d - (Int32)LeadingZeroCount((%(T)s)(~%(v)s));
    }''' % dict(T=T, v=w('value'), zero=zero, bits=bits, bits1=bits + 1))
    else:
        add('''
    public Int32 GetShortestBitLength() => %(bits)d - (Int32)LeadingZeroCount(this);''' % dict(T=T, bits=bits))

    # --- the conversions from and to the other numbers (see Numerics.IntegerConversions)
    cast = '(%s)(Int64)bits' % T if signed else '(%s)bits' % T
    for mode in ('Checked', 'Saturating', 'Truncating'):
        if mode == 'Checked':
            convert = """        if (!Numerics.IntegerConversions.Fits(negative, magnitude, %(bits)d, %(signed)s))
            throw new OverflowException();

        UInt64 bits = Numerics.IntegerConversions.Truncate(negative, magnitude);"""
        elif mode == 'Saturating':
            convert = """        UInt64 bits = Numerics.IntegerConversions.Saturate(negative, magnitude, %(bits)d, %(signed)s);"""
        else:
            convert = """        UInt64 bits = Numerics.IntegerConversions.Truncate(negative, magnitude);"""

        add("""
    public static Boolean TryConvertFrom%(mode)s<TOther>(TOther value, out %(T)s result)
        where TOther : Numerics.INumberBase<TOther>
    {
        Boolean negative;
        UInt64 magnitude;

        if (!Numerics.IntegerConversions.TryRead(value, out negative, out magnitude))
        {
            result = (%(T)s)0;
            return false;
        }

""" % dict(mode=mode, T=T) + (convert % dict(bits=bits, signed='true' if signed else 'false')) + """

        result = %(cast)s;
        return true;
    }

    public static Boolean TryConvertTo%(mode)s<TTarget>(%(T)s value, out TTarget result)
        where TTarget : Numerics.INumberBase<TTarget> => TTarget.TryConvertFrom%(mode)s<%(T)s>(value, out result);""" % dict(mode=mode, T=T, cast=cast))

    return '\n\n'.join(out)


def update_type_file(spec):
    name = spec[0]
    path = os.path.join(LIB, name + '.cs')
    text = open(path, encoding='utf-8', newline='').read()
    nl = '\r\n' if '\r\n' in text else '\n'
    body = members(*spec).replace('\n', nl)

    block = BEGIN + nl + nl + body + nl + nl + END

    if BEGIN in text:
        i = text.index(BEGIN)
        j = text.index(END) + len(END)
        text = text[:i] + block + text[j:]
    else:
        # Before the } that ends the struct (the last one of the file).
        k = text.rstrip().rfind('}')
        text = text[:k].rstrip() + nl + nl + block + nl + text[k:]

    open(path, 'w', encoding='utf-8', newline='').write(text)


def update_types_file():
    path = os.path.join(LIB, '+types.cs')
    text = open(path, encoding='utf-8', newline='').read()

    for name, bits, signed, unsigned, minimum, maximum in TYPES:
        number = 'ISignedNumber' if signed else 'IUnsignedNumber'
        wanted = ('public partial struct %(T)s : IComparable<%(T)s>, IEquatable<%(T)s>, IEquality, IStringable, IFormattable, '
                  'Numerics.IBinaryInteger<%(T)s>, IMinMaxValue<%(T)s>, Numerics.%(N)s<%(T)s>') % dict(T=name, N=number)

        pattern = re.compile(r'public partial struct %s : [^\r\n]*' % name)
        assert pattern.search(text), name
        text = pattern.sub(lambda m: wanted, text, count=1)

    open(path, 'w', encoding='utf-8', newline='').write(text)


if __name__ == '__main__':
    for spec in TYPES:
        update_type_file(spec)

    update_types_file()
    print('done')
