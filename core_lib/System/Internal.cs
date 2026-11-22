
/*
 *  System.dll
 *
 */

////////// ////////// ////////// ////////// //////////

using Reflection;

////////// ////////// ////////// ////////// //////////

internal static class Internal
{

    // Object

    [Internal]
    public static extern Object Object_FromPtr(Ptr ptr);


    // Ptr

    [Internal]
    public static extern Ptr Ptr_FromInt64(Int64 value);

    [Internal]
    public static extern Int64 Ptr_ToInt64(Ptr ptr);


    // Memory (the memory of the program, not of the heap that is collected)

    [Internal]
    public static extern Ptr Memory_Alloc(Int64 bytes);

    [Internal]
    public static extern Void Memory_Free(Ptr ptr);


    // Time

    // The clock of the system, UTC, in ticks of 100 nanoseconds since 0001-01-01 (the ticks of DateTime and TimeSpan).
    [Internal]
    public static extern Int64 Time_UtcNowTicks();

    // A clock that only goes on, in ticks of 100 nanoseconds; its zero is not defined.
    [Internal]
    public static extern Int64 Time_MonotonicTicks();

    // Copies the bytes (the ranges may overlap), fills them.
    [Internal]
    public static extern Void Memory_Copy(Ptr destination, Ptr source, Int64 bytes);

    [Internal]
    public static extern Void Memory_Fill(Ptr destination, UInt8 value, Int64 bytes);

    // The address of the character of a string, the index is checked by the caller (null for a string that is null).
    [Internal]
    public static extern Ptr String_AddressOf(String s, Int32 index);


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

    // Sets the items of the tuple that is made by the constructor that calls it (its this).
    [Internal]
    public static extern Void Tuple_SetRange(Type type, Ptr data);

    // Makes a tuple in the heap, from the items.
    [Internal]
    public static extern Object Tuple_Create(Type type, Ptr data);

    // Returns the item of the tuple that is the this of the method that calls it.
    [Internal]
    public static extern Object Tuple_Get(Type type, Int32 index);

    [Internal]
    public static extern Int32 Tuple_GetCount(Type type);


    // String

    [Internal]
    public static extern String String_Concat(String a, String b);

    [Internal]
    public static extern Boolean String_Equals(String a, String b);

    [Internal]
    public static extern Int32 String_Compare(String a, String b);

    [Internal]
    public static extern Int32 String_GetLength(String s);

    [Internal]
    public static extern Int32 String_GetHashCode(String s);

    [Internal]
    public static extern String String_Substring(String s, Int32 start, Int32 length);

    [Internal]
    public static extern Int32 Object_GetHashCode(Object obj);

    // The number of a boxed enum, of its underlying type.
    [Internal]
    public static extern Int64 Enum_GetNumber(Object? value);

    // The text of a boxed enum: the name of its item, the number when it has no item.
    [Internal]
    public static extern String Enum_ToString(Object value);

    // The box of the value of a text (a name of an item, a number, or the ones separated by commas) of an enum, null
    // when the text is not a value of it.
    [Internal]
    public static extern Object? Enum_Parse(Type type, String text, Boolean ignoreCase);

    [Internal]
    public static extern Boolean Object_ReferenceEquals(Object? obj1, Object? obj2);

    [Internal]
    public static extern String Int64_ToString(Int64 value);

    [Internal]
    public static extern String UInt64_ToString(UInt64 value);

    [Internal]
    public static extern String Double_ToString(Double value);

    // The text of the number with the number of digits after the point: fixed (1234.50), or scientific (1.234e+03).
    [Internal]
    public static extern String Double_ToFixed(Double value, Int32 digits);

    [Internal]
    public static extern String Double_ToScientific(Double value, Int32 digits);

    [Internal]
    public static extern String Single_ToString(Single value);

    [Internal]
    public static extern String Char_ToString(Char value);

    [Internal]
    public static extern Char String_GetChar(String s, Int32 index);

    // A string of the characters of the array from the start.
    [Internal]
    public static extern String String_FromChars(Char[] chars, Int32 start, Int32 count);

    // Copies the characters of the string from the source index to the array from the start.
    [Internal]
    public static extern Void String_CopyTo(String s, Int32 source, Char[] chars, Int32 start, Int32 count);

    [Internal]
    public static extern Int32 String_IndexOf(String s, String value, Int32 start);

    // The last position of the value that ends at or before the end.
    [Internal]
    public static extern Int32 String_LastIndexOf(String s, String value, Int32 end);

    [Internal]
    public static extern Int32 String_IndexOfChar(String s, Char value, Int32 start);

    [Internal]
    public static extern Int32 String_LastIndexOfChar(String s, Char value);

    [Internal]
    public static extern String String_Trim(String s, Int32 sides);

    [Internal]
    public static extern Boolean String_IsBlank(String s);

    [Internal]
    public static extern Int32 Utf8_GetByteCount(String s);

    [Internal]
    public static extern Int32 Utf8_GetBytes(String s, UInt8[] bytes, Int32 index);

    // The number of bytes of the resource of the name (it is -1 when no assembly has it), and the bytes of it.
    [Internal]
    public static extern Int32 Resource_GetSize(String name);

    // The address of the bytes of the resource: they are kept by the program (null when there is none).
    [Internal]
    public static extern Ptr Resource_GetPtr(String name);

    [Internal]
    public static extern Void Resource_Read(String name, UInt8[] destination);

    [Internal]
    public static extern String Utf8_GetString(UInt8[] bytes, Int32 index, Int32 count);

    [Internal]
    public static extern String String_ToUpper(String s);

    [Internal]
    public static extern String String_ToLower(String s);

    [Internal]
    public static extern Boolean Char_IsLetter(Char value);

    [Internal]
    public static extern Boolean Char_IsDigit(Char value);

    [Internal]
    public static extern Boolean Char_IsWhiteSpace(Char value);

    [Internal]
    public static extern Char Char_ToUpper(Char value);

    [Internal]
    public static extern Char Char_ToLower(Char value);


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

    [Internal]
    public static extern String Reflection_GetTypeFullName(Object obj);

    // The members of the types: the types and the members are handles (they are not objects of the
    // heap), the tables of them are built by the runtime. kind: 0 field, 1 method, 2 property,
    // 3 event, 4 generic parameter.

    [Internal]
    public static extern Type? Reflection_GetTypeBase(Type type);

    [Internal]
    public static extern Int64 Reflection_GetTypeFlags(Type type);

    [Internal]
    public static extern Assembly? Reflection_GetTypeAssembly(Type type);

    [Internal]
    public static extern Int32 Reflection_GetMemberCount(Type type, Int32 kind);

    [Internal]
    public static extern Field Reflection_GetField(Type type, Int32 index);

    public static extern Int32 Reflection_GetNestedTypeCount(Type type);

    public static extern Type Reflection_GetNestedType(Type type, Int32 index);

    [Internal]
    public static extern Method Reflection_GetMethod(Type type, Int32 index);

    [Internal]
    public static extern Property Reflection_GetProperty(Type type, Int32 index);

    [Internal]
    public static extern Event Reflection_GetEvent(Type type, Int32 index);

    [Internal]
    public static extern GenericParameter Reflection_GetTypeGenericParameter(Type type, Int32 index);

    [Internal]
    public static extern String Reflection_GetInfoName(ReflectionInfo info);

    [Internal]
    public static extern Type Reflection_GetInfoDeclaringType(ReflectionInfo info);

    [Internal]
    public static extern Type? Reflection_GetInfoType(ReflectionInfo info);

    [Internal]
    public static extern Int64 Reflection_GetInfoFlags(ReflectionInfo info);

    [Internal]
    public static extern Method? Reflection_GetPropertyGetter(Property property);

    [Internal]
    public static extern Method? Reflection_GetPropertySetter(Property property);

    [Internal]
    public static extern Method? Reflection_GetEventAdder(Event e);

    [Internal]
    public static extern Method? Reflection_GetEventRemover(Event e);

    [Internal]
    public static extern Method? Reflection_GetEventRaiser(Event e);

    [Internal]
    public static extern Int32 Reflection_CheckFieldAccess(Field field, Object? obj, Object? value, Int32 mode);

    [Internal]
    public static extern Object? Reflection_GetFieldValue(Field field, Object? obj);

    [Internal]
    public static extern Void Reflection_SetFieldValue(Field field, Object? obj, Object? value);

    // Whether the object of reflection is a member, a parameter or an assembly (1), and not a Type (0).
    [Internal]
    public static extern Int32 Reflection_IsMember(ReflectionInfo info);

    // The attributes of a member (or of a type): their count, the constructor of one of them, and
    // the arguments of that constructor (boxed, of the types of its parameters).

    [Internal]
    public static extern Int32 Reflection_GetAttributeCount(ReflectionInfo info);

    [Internal]
    public static extern Method? Reflection_GetAttributeConstructor(ReflectionInfo info, Int32 index);

    [Internal]
    public static extern Int32 Reflection_GetAttributeArgumentCount(ReflectionInfo info, Int32 index);

    [Internal]
    public static extern Object? Reflection_GetAttributeArgument(ReflectionInfo info, Int32 index, Int32 position);

    // The named arguments ([Author("ann", Level = 3)]): their count, the name of the field or of
    // the property, and the value (boxed in its type).

    [Internal]
    public static extern Int32 Reflection_GetAttributeNamedCount(ReflectionInfo info, Int32 index);

    [Internal]
    public static extern String Reflection_GetAttributeNamedName(ReflectionInfo info, Int32 index, Int32 position);

    [Internal]
    public static extern Object? Reflection_GetAttributeNamedValue(ReflectionInfo info, Int32 index, Int32 position);

    [Internal]
    public static extern Int32 Reflection_GetTypeAttributeNamedCount(ReflectionInfo type, Int32 index);

    [Internal]
    public static extern String Reflection_GetTypeAttributeNamedName(ReflectionInfo type, Int32 index, Int32 position);

    [Internal]
    public static extern Object? Reflection_GetTypeAttributeNamedValue(ReflectionInfo type, Int32 index, Int32 position);

    [Internal]
    public static extern Int32 Reflection_GetTypeAttributeCount(ReflectionInfo type);

    [Internal]
    public static extern Method? Reflection_GetTypeAttributeConstructor(ReflectionInfo type, Int32 index);

    [Internal]
    public static extern Int32 Reflection_GetTypeAttributeArgumentCount(ReflectionInfo type, Int32 index);

    [Internal]
    public static extern Object? Reflection_GetTypeAttributeArgument(ReflectionInfo type, Int32 index, Int32 position);

    // Makes an object of a class that no constructor has run for.
    [Internal]
    public static extern Object Reflection_NewObject(Type type);

    // Checks the call of a method: 0 if it can be done, 1 if the object is null, 2 if the object or
    // an argument are not of the types that are needed (the interpreter checks while it calls).
    [Internal]
    public static extern Int32 Reflection_CheckInvoke(Method method, Object? target, Object? args);

    // Calls the method of the object with the arguments (each of them is the object that is passed,
    // or the box of the value that is passed); returns what the method returns, in the same way.
    [Internal]
    public static extern Object? Reflection_Invoke(Method method, Object? target, Array<Object?> args);

    [Internal]
    public static extern Int32 Reflection_GetParameterCount(ReflectionInfo info);

    [Internal]
    public static extern Parameter Reflection_GetParameter(ReflectionInfo info, Int32 index);

    [Internal]
    public static extern Int32 Reflection_GetGenericParameterCount(Method method);

    [Internal]
    public static extern GenericParameter Reflection_GetGenericParameter(Method method, Int32 index);


    // Delegate

    [Internal]
    public static extern T Delegate_Invoke<T>();

    [Internal]
    public static extern Void Delegate_Init();

    [Internal]
    public static extern Void Delegate_InitWithCallType();

    [Internal]
    public static extern Int32 MulticastDelegate_GetCount();

    [Internal]
    public static extern TDelegate MulticastDelegate_Get<TDelegate>(Int32 index);

    // [Internal]
    // public static extern TMulticastDelegate MulticastDelegate_Add<TMulticastDelegate>(
    //                         TMulticastDelegate self, TMulticastDelegate.Self func);

    // [Internal]
    // public static extern TMulticastDelegate MulticastDelegate_Remove<TMulticastDelegate>(
    //                         TMulticastDelegate self, TMulticastDelegate.Self func);

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

    // Threads: see thread_manager.cpp. The time is in milliseconds, -1 is for ever.

    // Starts the thread of the object (a System.Threading.Thread, that runs its __Entry), returns its id.
    [Internal]
    public static extern Int64 Thread_Start(Object thread, Boolean background);

    [Internal]
    public static extern Boolean Thread_Join(Int64 id, Int32 timeout);

    [Internal]
    public static extern Void Thread_Sleep(Int32 milliseconds);

    [Internal]
    public static extern Void Thread_Yield();

    [Internal]
    public static extern Int64 Thread_CurrentId();

    // The object of the running thread, null for the thread that started the program.
    [Internal]
    public static extern Object? Thread_GetCurrent();

    [Internal]
    public static extern Boolean Thread_IsAlive(Int64 id);

    [Internal]
    public static extern Void Thread_SetBackground(Int64 id, Boolean background);

    [Internal]
    public static extern Boolean Thread_IsBackground(Int64 id);

    // The lock of an object.
    [Internal]
    public static extern Void Monitor_Enter(Object obj);

    [Internal]
    public static extern Boolean Monitor_TryEnter(Object obj, Int32 timeout);

    // 0 when the lock is released, 1 when the thread does not hold it.
    [Internal]
    public static extern Int32 Monitor_Exit(Object obj);

    // 0 when the thread does not hold the lock, 1 when it was pulsed, 2 when the time was over.
    [Internal]
    public static extern Int32 Monitor_Wait(Object obj, Int32 timeout);

    // Returns whether the thread holds the lock.
    [Internal]
    public static extern Boolean Monitor_Pulse(Object obj, Boolean all);

    [Internal]
    public static extern Boolean Monitor_IsEntered(Object obj);

    // The processors of the machine.
    [Internal]
    public static extern Int32 Environment_ProcessorCount();

    // Whether the code of the threads runs in parallel (the native programs): the numbers of Interlocked are atomic instructions.
    [Internal]
    public static extern Boolean Thread_IsParallel();

    // The operations of Interlocked: they work on the place of a number, Add returns the new value, the others the old one.
    [Internal]
    public static extern Int32 Interlocked_Add32(ref Int32 location, Int32 value);

    [Internal]
    public static extern Int64 Interlocked_Add64(ref Int64 location, Int64 value);

    [Internal]
    public static extern Int32 Interlocked_Exchange32(ref Int32 location, Int32 value);

    [Internal]
    public static extern Int64 Interlocked_Exchange64(ref Int64 location, Int64 value);

    [Internal]
    public static extern Int32 Interlocked_CompareExchange32(ref Int32 location, Int32 value, Int32 comparand);

    [Internal]
    public static extern Int64 Interlocked_CompareExchange64(ref Int64 location, Int64 value, Int64 comparand);
}


