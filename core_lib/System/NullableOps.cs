/*
 *  System.dll
 *
 *  The operators of the nullable numbers (a + b of Int32? and Int32): the compiler calls Invoke of the class of the operator, of
 *  the type of the numbers (NullableAddOperator<Int32>), for the operators that the nullable types do not declare. The result has
 *  no value when an operand has none; a comparison is false then, and two operands that have no value are equal.
 */

using Numerics;

////////// ////////// ////////// ////////// //////////

public static class NullableAddOperator<T> where T : IAdditionOperators<T, T, T>
{
    public static Nullable<T> Invoke(Nullable<T> left, Nullable<T> right)
    {
        if (left.HasValue && right.HasValue)
            return new Nullable<T>(left.Value + right.Value);

        return default(Nullable<T>);
    }
}

public static class NullableSubtractOperator<T> where T : ISubtractionOperators<T, T, T>
{
    public static Nullable<T> Invoke(Nullable<T> left, Nullable<T> right)
    {
        if (left.HasValue && right.HasValue)
            return new Nullable<T>(left.Value - right.Value);

        return default(Nullable<T>);
    }
}

public static class NullableMultiplyOperator<T> where T : IMultiplyOperators<T, T, T>
{
    public static Nullable<T> Invoke(Nullable<T> left, Nullable<T> right)
    {
        if (left.HasValue && right.HasValue)
            return new Nullable<T>(left.Value * right.Value);

        return default(Nullable<T>);
    }
}

public static class NullableDivideOperator<T> where T : IDivisionOperators<T, T, T>
{
    public static Nullable<T> Invoke(Nullable<T> left, Nullable<T> right)
    {
        if (left.HasValue && right.HasValue)
            return new Nullable<T>(left.Value / right.Value);

        return default(Nullable<T>);
    }
}

public static class NullableModulusOperator<T> where T : IModulusOperators<T, T, T>
{
    public static Nullable<T> Invoke(Nullable<T> left, Nullable<T> right)
    {
        if (left.HasValue && right.HasValue)
            return new Nullable<T>(left.Value % right.Value);

        return default(Nullable<T>);
    }
}

public static class NullableBitAndOperator<T> where T : IBitwiseOperators<T, T, T>
{
    public static Nullable<T> Invoke(Nullable<T> left, Nullable<T> right)
    {
        if (left.HasValue && right.HasValue)
            return new Nullable<T>(left.Value & right.Value);

        return default(Nullable<T>);
    }
}

public static class NullableBitOrOperator<T> where T : IBitwiseOperators<T, T, T>
{
    public static Nullable<T> Invoke(Nullable<T> left, Nullable<T> right)
    {
        if (left.HasValue && right.HasValue)
            return new Nullable<T>(left.Value | right.Value);

        return default(Nullable<T>);
    }
}

public static class NullableBitXorOperator<T> where T : IBitwiseOperators<T, T, T>
{
    public static Nullable<T> Invoke(Nullable<T> left, Nullable<T> right)
    {
        if (left.HasValue && right.HasValue)
            return new Nullable<T>(left.Value ^ right.Value);

        return default(Nullable<T>);
    }
}

public static class NullableLeftShiftOperator<T> where T : IShiftOperators<T, Int32, T>
{
    public static Nullable<T> Invoke(Nullable<T> value, Int32 count)
    {
        if (value.HasValue)
            return new Nullable<T>(value.Value << count);

        return default(Nullable<T>);
    }
}

public static class NullableRightShiftOperator<T> where T : IShiftOperators<T, Int32, T>
{
    public static Nullable<T> Invoke(Nullable<T> value, Int32 count)
    {
        if (value.HasValue)
            return new Nullable<T>(value.Value >> count);

        return default(Nullable<T>);
    }
}

public static class NullableNegateOperator<T> where T : IUnaryNegationOperators<T, T>
{
    public static Nullable<T> Invoke(Nullable<T> value)
    {
        if (value.HasValue)
            return new Nullable<T>(-value.Value);

        return default(Nullable<T>);
    }
}

public static class NullableComplementOperator<T> where T : IBitwiseOperators<T, T, T>
{
    public static Nullable<T> Invoke(Nullable<T> value)
    {
        if (value.HasValue)
            return new Nullable<T>(~value.Value);

        return default(Nullable<T>);
    }
}

public static class NullableLessThanOperator<T> where T : IComparisonOperators<T, T, Boolean>
{
    public static Boolean Invoke(Nullable<T> left, Nullable<T> right)
    {
        return left.HasValue && right.HasValue && left.Value < right.Value;
    }
}

public static class NullableLessThanOrEqualOperator<T> where T : IComparisonOperators<T, T, Boolean>
{
    public static Boolean Invoke(Nullable<T> left, Nullable<T> right)
    {
        return left.HasValue && right.HasValue && left.Value <= right.Value;
    }
}

public static class NullableGreaterThanOperator<T> where T : IComparisonOperators<T, T, Boolean>
{
    public static Boolean Invoke(Nullable<T> left, Nullable<T> right)
    {
        return left.HasValue && right.HasValue && left.Value > right.Value;
    }
}

public static class NullableGreaterThanOrEqualOperator<T> where T : IComparisonOperators<T, T, Boolean>
{
    public static Boolean Invoke(Nullable<T> left, Nullable<T> right)
    {
        return left.HasValue && right.HasValue && left.Value >= right.Value;
    }
}

public static class NullableEqualOperator<T> where T : IEqualityOperators<T, T, Boolean>
{
    public static Boolean Invoke(Nullable<T> left, Nullable<T> right)
    {
        if (left.HasValue != right.HasValue)
            return false;

        return !left.HasValue || left.Value == right.Value;
    }
}

public static class NullableNotEqualOperator<T> where T : IEqualityOperators<T, T, Boolean>
{
    public static Boolean Invoke(Nullable<T> left, Nullable<T> right)
    {
        if (left.HasValue != right.HasValue)
            return true;

        return left.HasValue && left.Value != right.Value;
    }
}
