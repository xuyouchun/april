/*
 *  System.dll
 *
 *  The interfaces of the numbers (generic math): the operators, the identities, the limits, and the
 *  interfaces of the numbers (INumberBase, INumber, IBinaryNumber, IBinaryInteger, ISignedNumber,
 *  IUnsignedNumber) that the primitive types implement. The members that are static are static abstract:
 *  they are used through a generic parameter that has the interface as a constraint (T.Zero, a + b).
 *
 *  What is not here yet: the checked operators, the conversions between the numbers (CreateChecked,
 *  CreateSaturating, CreateTruncating, TryConvertFrom, TryConvertTo), the parsing and the formatting, the
 *  spans, and the unsigned right shift (>>>) of IShiftOperators: they need the language, or the library, to
 *  have them.
 */

////////// ////////// ////////// ////////// //////////

// The type has a lowest and a highest value.
public interface IMinMaxValue<TSelf>
    where TSelf : IMinMaxValue<TSelf>
{
    static abstract TSelf MinValue { get; }

    static abstract TSelf MaxValue { get; }
}

////////// ////////// ////////// ////////// //////////

namespace Numerics
{
    // The operators.

    public interface IAdditionOperators<TSelf, TOther, TResult>
        where TSelf : IAdditionOperators<TSelf, TOther, TResult>
    {
        static abstract TResult operator +(TSelf left, TOther right);

        // The operator in a checked context: it raises OverflowException when the result does not fit. The
        // one that is not checked is used when a type has no one of its own.
        static virtual TResult operator checked +(TSelf left, TOther right) { return left + right; }
    }

    public interface ISubtractionOperators<TSelf, TOther, TResult>
        where TSelf : ISubtractionOperators<TSelf, TOther, TResult>
    {
        static abstract TResult operator -(TSelf left, TOther right);

        // The operator in a checked context: it raises OverflowException when the result does not fit. The
        // one that is not checked is used when a type has no one of its own.
        static virtual TResult operator checked -(TSelf left, TOther right) { return left - right; }
    }

    public interface IMultiplyOperators<TSelf, TOther, TResult>
        where TSelf : IMultiplyOperators<TSelf, TOther, TResult>
    {
        static abstract TResult operator *(TSelf left, TOther right);

        // The operator in a checked context: it raises OverflowException when the result does not fit. The
        // one that is not checked is used when a type has no one of its own.
        static virtual TResult operator checked *(TSelf left, TOther right) { return left * right; }
    }

    public interface IDivisionOperators<TSelf, TOther, TResult>
        where TSelf : IDivisionOperators<TSelf, TOther, TResult>
    {
        static abstract TResult operator /(TSelf left, TOther right);

        // The operator in a checked context: it raises OverflowException when the result does not fit. The
        // one that is not checked is used when a type has no one of its own.
        static virtual TResult operator checked /(TSelf left, TOther right) { return left / right; }
    }

    public interface IModulusOperators<TSelf, TOther, TResult>
        where TSelf : IModulusOperators<TSelf, TOther, TResult>
    {
        static abstract TResult operator %(TSelf left, TOther right);
    }

    public interface IUnaryPlusOperators<TSelf, TResult>
        where TSelf : IUnaryPlusOperators<TSelf, TResult>
    {
        static abstract TResult operator +(TSelf value);
    }

    public interface IUnaryNegationOperators<TSelf, TResult>
        where TSelf : IUnaryNegationOperators<TSelf, TResult>
    {
        static abstract TResult operator -(TSelf value);

        static virtual TResult operator checked -(TSelf value) { return -value; }
    }

    public interface IIncrementOperators<TSelf>
        where TSelf : IIncrementOperators<TSelf>
    {
        static abstract TSelf operator ++(TSelf value);

        static virtual TSelf operator checked ++(TSelf value) { TSelf result = value; result++; return result; }
    }

    public interface IDecrementOperators<TSelf>
        where TSelf : IDecrementOperators<TSelf>
    {
        static abstract TSelf operator --(TSelf value);

        static virtual TSelf operator checked --(TSelf value) { TSelf result = value; result--; return result; }
    }

    public interface IBitwiseOperators<TSelf, TOther, TResult>
        where TSelf : IBitwiseOperators<TSelf, TOther, TResult>
    {
        static abstract TResult operator &(TSelf left, TOther right);

        static abstract TResult operator |(TSelf left, TOther right);

        static abstract TResult operator ^(TSelf left, TOther right);

        static abstract TResult operator ~(TSelf value);
    }

    // The shift count is the TOther (an Int32 for the numbers). There is no >>> yet.
    public interface IShiftOperators<TSelf, TOther, TResult>
        where TSelf : IShiftOperators<TSelf, TOther, TResult>
    {
        static abstract TResult operator <<(TSelf value, TOther shiftAmount);

        static abstract TResult operator >>(TSelf value, TOther shiftAmount);
    }

    public interface IEqualityOperators<TSelf, TOther, TResult>
        where TSelf : IEqualityOperators<TSelf, TOther, TResult>
    {
        static abstract TResult operator ==(TSelf left, TOther right);

        static abstract TResult operator !=(TSelf left, TOther right);
    }

    public interface IComparisonOperators<TSelf, TOther, TResult> : IEqualityOperators<TSelf, TOther, TResult>
        where TSelf : IComparisonOperators<TSelf, TOther, TResult>
    {
        static abstract TResult operator <(TSelf left, TOther right);

        static abstract TResult operator >(TSelf left, TOther right);

        static abstract TResult operator <=(TSelf left, TOther right);

        static abstract TResult operator >=(TSelf left, TOther right);
    }

    ////////// ////////// ////////// ////////// //////////

    // The identities: a + AdditiveIdentity is a, a * MultiplicativeIdentity is a.

    public interface IAdditiveIdentity<TSelf, TResult>
        where TSelf : IAdditiveIdentity<TSelf, TResult>
    {
        static abstract TResult AdditiveIdentity { get; }
    }

    public interface IMultiplicativeIdentity<TSelf, TResult>
        where TSelf : IMultiplicativeIdentity<TSelf, TResult>
    {
        static abstract TResult MultiplicativeIdentity { get; }
    }

    ////////// ////////// ////////// ////////// //////////

    // The members of all the numbers.
    public interface INumberBase<TSelf> :
            IAdditionOperators<TSelf, TSelf, TSelf>,
            IAdditiveIdentity<TSelf, TSelf>,
            IDecrementOperators<TSelf>,
            IDivisionOperators<TSelf, TSelf, TSelf>,
            IEquatable<TSelf>,
            IEqualityOperators<TSelf, TSelf, Boolean>,
            IIncrementOperators<TSelf>,
            IMultiplicativeIdentity<TSelf, TSelf>,
            IMultiplyOperators<TSelf, TSelf, TSelf>,
            ISubtractionOperators<TSelf, TSelf, TSelf>,
            IUnaryNegationOperators<TSelf, TSelf>,
            IUnaryPlusOperators<TSelf, TSelf>
        where TSelf : INumberBase<TSelf>
    {
        static abstract TSelf One { get; }

        static abstract Int32 Radix { get; }

        static abstract TSelf Zero { get; }

        static abstract TSelf Abs(TSelf value);

        static abstract Boolean IsCanonical(TSelf value);

        static abstract Boolean IsComplexNumber(TSelf value);

        static abstract Boolean IsEvenInteger(TSelf value);

        static abstract Boolean IsFinite(TSelf value);

        static abstract Boolean IsImaginaryNumber(TSelf value);

        static abstract Boolean IsInfinity(TSelf value);

        static abstract Boolean IsInteger(TSelf value);

        static abstract Boolean IsNaN(TSelf value);

        static abstract Boolean IsNegative(TSelf value);

        static abstract Boolean IsNegativeInfinity(TSelf value);

        static abstract Boolean IsNormal(TSelf value);

        static abstract Boolean IsOddInteger(TSelf value);

        static abstract Boolean IsPositive(TSelf value);

        static abstract Boolean IsPositiveInfinity(TSelf value);

        static abstract Boolean IsRealNumber(TSelf value);

        static abstract Boolean IsSubnormal(TSelf value);

        static abstract Boolean IsZero(TSelf value);

        // The number of the value of a number of another type: CreateChecked raises OverflowException when it
        // does not fit, CreateSaturating takes the lowest or the highest value, CreateTruncating takes the
        // low bits. A type that cannot convert the other one (TryConvertFrom*), or that the other one cannot
        // convert to (TryConvertTo*), makes NotSupportedException.
        static virtual TSelf CreateChecked<TOther>(TOther value) where TOther : INumberBase<TOther>
        {
            TSelf result;
            if (TSelf.TryConvertFromChecked<TOther>(value, out result))
                return result;

            if (TOther.TryConvertToChecked<TSelf>(value, out result))
                return result;

            throw new NotSupportedException();
        }

        static virtual TSelf CreateSaturating<TOther>(TOther value) where TOther : INumberBase<TOther>
        {
            TSelf result;
            if (TSelf.TryConvertFromSaturating<TOther>(value, out result))
                return result;

            if (TOther.TryConvertToSaturating<TSelf>(value, out result))
                return result;

            throw new NotSupportedException();
        }

        static virtual TSelf CreateTruncating<TOther>(TOther value) where TOther : INumberBase<TOther>
        {
            TSelf result;
            if (TSelf.TryConvertFromTruncating<TOther>(value, out result))
                return result;

            if (TOther.TryConvertToTruncating<TSelf>(value, out result))
                return result;

            throw new NotSupportedException();
        }

        // Converts the value of another type to this one; false if the type does not know the other one.
        static abstract Boolean TryConvertFromChecked<TOther>(TOther value, out TSelf result)
            where TOther : INumberBase<TOther>;

        static abstract Boolean TryConvertFromSaturating<TOther>(TOther value, out TSelf result)
            where TOther : INumberBase<TOther>;

        static abstract Boolean TryConvertFromTruncating<TOther>(TOther value, out TSelf result)
            where TOther : INumberBase<TOther>;

        // Converts the value of this type to another one; false if the type does not know the other one.
        static abstract Boolean TryConvertToChecked<TOther>(TSelf value, out TOther result)
            where TOther : INumberBase<TOther>;

        static abstract Boolean TryConvertToSaturating<TOther>(TSelf value, out TOther result)
            where TOther : INumberBase<TOther>;

        static abstract Boolean TryConvertToTruncating<TOther>(TSelf value, out TOther result)
            where TOther : INumberBase<TOther>;

        // The value that has the larger absolute value; when they are equal, the positive one.
        static abstract TSelf MaxMagnitude(TSelf x, TSelf y);

        static abstract TSelf MaxMagnitudeNumber(TSelf x, TSelf y);

        // The value that has the smaller absolute value; when they are equal, the negative one.
        static abstract TSelf MinMagnitude(TSelf x, TSelf y);

        static abstract TSelf MinMagnitudeNumber(TSelf x, TSelf y);
    }

    // A number that is signed: it has -1.
    public interface ISignedNumber<TSelf> : INumberBase<TSelf>
        where TSelf : ISignedNumber<TSelf>
    {
        static abstract TSelf NegativeOne { get; }
    }

    // A number that is not signed.
    public interface IUnsignedNumber<TSelf> : INumberBase<TSelf>
        where TSelf : IUnsignedNumber<TSelf>
    {
    }

    ////////// ////////// ////////// ////////// //////////

    // The numbers that are ordered.
    public interface INumber<TSelf> :
            IComparable<TSelf>,
            IComparisonOperators<TSelf, TSelf, Boolean>,
            IModulusOperators<TSelf, TSelf, TSelf>,
            INumberBase<TSelf>
        where TSelf : INumber<TSelf>
    {
        // Returns -1, 0 or 1: the sign of the value.
        static abstract Int32 Sign(TSelf value);

        // The value in the range from min to max; min must not be larger than max.
        static virtual TSelf Clamp(TSelf value, TSelf min, TSelf max)
        {
            if (min > max)
                throw new ArgumentException();

            TSelf result = value;
            if (result < min)
                result = min;

            if (result > max)
                result = max;

            return result;
        }

        // The magnitude of the value with the sign of sign.
        static virtual TSelf CopySign(TSelf value, TSelf sign)
        {
            TSelf result = value;

            if (TSelf.IsNegative(value) != TSelf.IsNegative(sign))
            {
                result = -value;

                // The negation of the lowest value of a signed type is itself (it is still negative).
                if (TSelf.IsNegative(result) != TSelf.IsNegative(sign))
                    throw new OverflowException();
            }

            return result;
        }

        static virtual TSelf Max(TSelf x, TSelf y)
        {
            if (TSelf.IsNaN(x))
                return x;

            return x > y? x : y;
        }

        static virtual TSelf MaxNumber(TSelf x, TSelf y)
        {
            if (TSelf.IsNaN(y))
                return x;

            return x > y? x : y;
        }

        static virtual TSelf Min(TSelf x, TSelf y)
        {
            if (TSelf.IsNaN(x))
                return x;

            return x < y? x : y;
        }

        static virtual TSelf MinNumber(TSelf x, TSelf y)
        {
            if (TSelf.IsNaN(y))
                return x;

            return x < y? x : y;
        }
    }

    ////////// ////////// ////////// ////////// //////////

    // The numbers that are binary (they are bits).
    public interface IBinaryNumber<TSelf> :
            IBitwiseOperators<TSelf, TSelf, TSelf>,
            INumber<TSelf>
        where TSelf : IBinaryNumber<TSelf>
    {
        // The value that has all the bits set.
        static abstract TSelf AllBitsSet { get; }

        // Whether the value is a power of 2.
        static abstract Boolean IsPow2(TSelf value);

        // The base 2 logarithm of the value, rounded down.
        static abstract TSelf Log2(TSelf value);
    }

    // The integers that are binary.
    public interface IBinaryInteger<TSelf> :
            IBinaryNumber<TSelf>,
            IShiftOperators<TSelf, Int32, TSelf>
        where TSelf : IBinaryInteger<TSelf>
    {
        // The quotient and the remainder of the division.
        static abstract Tuple<TSelf, TSelf> DivRem(TSelf left, TSelf right);

        // The number of the bits that are 0 before the first bit that is 1 (from the most significant bit).
        static abstract TSelf LeadingZeroCount(TSelf value);

        // The number of the bits that are 1.
        static abstract TSelf PopCount(TSelf value);

        static abstract TSelf RotateLeft(TSelf value, Int32 rotateAmount);

        static abstract TSelf RotateRight(TSelf value, Int32 rotateAmount);

        // The number of the bits that are 0 after the last bit that is 1 (from the least significant bit).
        static abstract TSelf TrailingZeroCount(TSelf value);

        // The number of the bytes of the value.
        Int32 GetByteCount();

        // The number of the bits that are needed to hold the value (with the sign, if it is signed).
        Int32 GetShortestBitLength();
    }
}
