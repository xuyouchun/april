
/*
 *  System.dll
 *
 */

////////// ////////// ////////// ////////// //////////

internal static class Internal
{

    // Object

    [Internal]
    public static extern Object Object_FromPtr(Ptr ptr);


    // Array

    [Internal]
    public static extern Int32 Array_GetRank(Array array);

    [Internal]
    public static extern Int32 Array_GetLength(Array array);

    [Internal]
    public static extern Int32 Array_GetLengthOfDimension(Array array, Int32 dimension);

    [Internal]
    public static extern T Array_GetValue<T>(Int32 index, Array<T> array);

    [Internal]
    public static extern Void Array_SetValue<T>(T value, Int32 index, Array<T> array);


    // Tuple

    [Internal]
    public static extern Void Tuple_SetRange(Type type, Object tuple, Ptr data);

    [Internal]
    public static extern Ptr Tuple_Get(Type type, Object tuple, Int32 index);

    [Internal]
    public static extern Int32 Tuple_GetCount(Type type);


    // Console

    [Internal]
    public static extern Void Console_WriteString(String value);

    [Internal]
    public static extern Void Console_WriteChar(Char value);

    [Internal]
    public static extern Void Console_WriteInt8(Int8 value);

    [Internal]
    public static extern Void Console_WriteUInt8(UInt8 value);

    [Internal]
    public static extern Void Console_WriteInt16(Int16 value);

    [Internal]
    public static extern Void Console_WriteUInt16(UInt16 value);

    [Internal]
    public static extern Void Console_WriteInt32(Int32 value);

    [Internal]
    public static extern Void Console_WriteUInt32(UInt32 value);

    [Internal]
    public static extern Void Console_WriteInt64(Int64 value);

    [Internal]
    public static extern Void Console_WriteUInt64(UInt64 value);

    [Internal]
    public static extern Void Console_WriteSingle(Single value);

    [Internal]
    public static extern Void Console_WriteDouble(Double value);

    [Internal]
    public static extern Void Console_WriteStringLine(String value);

    [Internal]
    public static extern Void Console_WriteCharLine(Char value);

    [Internal]
    public static extern Void Console_WriteInt8Line(Int8 value);

    [Internal]
    public static extern Void Console_WriteUInt8Line(UInt8 value);

    [Internal]
    public static extern Void Console_WriteInt16Line(Int16 value);

    [Internal]
    public static extern Void Console_WriteUInt16Line(UInt16 value);

    [Internal]
    public static extern Void Console_WriteInt32Line(Int32 value);

    [Internal]
    public static extern Void Console_WriteUInt32Line(UInt32 value);

    [Internal]
    public static extern Void Console_WriteInt64Line(Int64 value);

    [Internal]
    public static extern Void Console_WriteUInt64Line(UInt64 value);

    [Internal]
    public static extern Void Console_WriteSingleLine(Single value);

    [Internal]
    public static extern Void Console_WriteDoubleLine(Double value);


    // Reflection

    [Internal]
    public static extern Type Reflection_GetType(Object obj);

    [Internal]
    public static extern String Reflection_GetTypeName(Object obj);


    // Delegate
    [Internal]
    public static extern T Delegate_Invoke<T>();

    [Internal]
    public static extern Void Delegate_Init();

    [Internal]
    public static extern Void Delegate_InitWithCallType();

    // Math

    [Internal]
    public static extern Double Math_Sqrt(Double val);

    [Internal]
    public static extern Double Math_Pow(Double x, Double y);

    [Internal]
    public static extern Double Math_Sin(Double val);

    [Internal]
    public static extern Double Math_Cos(Double val);

    [Internal]
    public static extern Double Math_Tan(Double val);

    [Internal]
    public static extern Double Math_Sinh(Double val);

    [Internal]
    public static extern Double Math_Cosh(Double val);

    [Internal]
    public static extern Double Math_Tanh(Double val);

    [Internal]
    public static extern Double Math_Asin(Double val);

    [Internal]
    public static extern Double Math_Acos(Double val);

    [Internal]
    public static extern Double Math_Atan(Double val);

    [Internal]
    public static extern Double Math_Asinh(Double val);

    [Internal]
    public static extern Double Math_Acosh(Double val);

    [Internal]
    public static extern Double Math_Atanh(Double val);

    [Internal]
    public static extern Double Math_Log(Double val);

    [Internal]
    public static extern Double Math_Log10(Double val);
}


