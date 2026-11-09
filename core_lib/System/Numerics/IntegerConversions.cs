/*
 *  System.dll
 *
 *  IntegerConversions: what the integer types need to convert a number of another type to themselves
 *  (CreateChecked, CreateSaturating, CreateTruncating, TryConvertFrom*).
 */

namespace Numerics;

////////// ////////// ////////// ////////// //////////

// A number is read as its sign and its magnitude (a UInt64): that holds the value of any integer type, so
// the conversions do not need a code for each pair of types. The integer types call these functions (the
// members of the types are made by tools/gen_numerics.py).
public static class IntegerConversions
{
    // Reads the value of an integer type. False if TOther is not one of the 8 integer types.
    public static Boolean TryRead<TOther>(TOther value, out Boolean negative, out UInt64 magnitude)
    {
        Object boxed = value;

        negative = false;
        magnitude = 0;

        if (boxed is Int32)
        {
            Int64 v = (Int64)(Int32)boxed;
            return __Signed(v, out negative, out magnitude);
        }

        if (boxed is UInt32)
        {
            magnitude = (UInt64)(UInt32)boxed;
            return true;
        }

        if (boxed is Int64)
            return __Signed((Int64)boxed, out negative, out magnitude);

        if (boxed is UInt64)
        {
            magnitude = (UInt64)boxed;
            return true;
        }

        if (boxed is Int8)
            return __Signed((Int64)(Int8)boxed, out negative, out magnitude);

        if (boxed is UInt8)
        {
            magnitude = (UInt64)(UInt8)boxed;
            return true;
        }

        if (boxed is Int16)
            return __Signed((Int64)(Int16)boxed, out negative, out magnitude);

        if (boxed is UInt16)
        {
            magnitude = (UInt64)(UInt16)boxed;
            return true;
        }

        return false;
    }

    // Whether the number fits an integer type of the bits (signed or not).
    public static Boolean Fits(Boolean negative, UInt64 magnitude, Int32 bits, Boolean signed)
    {
        if (negative)
        {
            if (magnitude == 0)
                return true;

            if (!signed)
                return false;

            // The magnitude of the lowest value is 2^(bits - 1).
            return magnitude <= ((UInt64)1 << (bits - 1));
        }

        UInt64 limit = signed? ((UInt64)1 << (bits - 1)) - 1 : (bits == 64? (UInt64)18446744073709551615 : ((UInt64)1 << bits) - 1);
        return magnitude <= limit;
    }

    // The bits of the number that is kept to the lowest or to the highest value of the type if it does not
    // fit (the bits are those of a UInt64, the two's complement of a negative number).
    public static UInt64 Saturate(Boolean negative, UInt64 magnitude, Int32 bits, Boolean signed)
    {
        if (Fits(negative, magnitude, bits, signed))
            return Truncate(negative, magnitude);

        if (negative)
            return signed? Truncate(true, (UInt64)1 << (bits - 1)) : 0;

        return signed? ((UInt64)1 << (bits - 1)) - 1 : (bits == 64? (UInt64)18446744073709551615 : ((UInt64)1 << bits) - 1);
    }

    // The bits of the number: the low bits of its two's complement.
    public static UInt64 Truncate(Boolean negative, UInt64 magnitude)
    {
        if (!negative)
            return magnitude;

        return ~magnitude + (UInt64)1;
    }

    private static Boolean __Signed(Int64 value, out Boolean negative, out UInt64 magnitude)
    {
        negative = value < 0;

        // The magnitude of the lowest Int64 is 2^63, that is -value in the bits.
        magnitude = negative? ~(UInt64)value + (UInt64)1 : (UInt64)value;
        return true;
    }
}
