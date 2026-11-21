/*
 *  System.dll
 *
 *  Object and Enum.
 */

using Reflection;

////////// ////////// ////////// ////////// //////////

[Internal]
public class Object
{
    // Returns whether the two are the same object.
    public static Boolean ReferenceEquals(Object? obj1, Object? obj2)
        => Internal.Object_ReferenceEquals(obj1, obj2);

    // Returns whether the two objects are equal: IEquality of the first one, or the same object
    // when its type does not have it (two nulls are equal).
    public static Boolean Equals(Object? obj1, Object? obj2)
    {
        if (obj1 == null)
            return obj2 == null;

        IEquality equality = obj1 as IEquality;
        return equality != null? equality.Equals(obj2) : Internal.Object_ReferenceEquals(obj1, obj2);
    }

    // Returns the hash code of the object: IEquality of it, or the identity of the object when
    // its type does not have it (the hash code of null is 0).
    public static Int32 HashOf(Object? obj)
    {
        if (obj == null)
            return 0;

        IEquality equality = obj as IEquality;
        return equality != null? equality.GetHashCode() : Internal.Object_GetHashCode(obj);
    }

    // Returns the text of the object: IStringable of it, or the full name of its type when the type
    // does not have it (the text of null is an empty string).
    public static String ToString(Object? obj)
    {
        if (obj == null)
            return "";

        IStringable stringable = obj as IStringable;
        if (stringable != null)
            return stringable.ToString();

        // An enum: the ToString that it declares, or the one of System.Enum (the name of the item).
        Type type = Type.GetType(obj);
        if (type.IsEnum)
        {
            Method? toString = type.GetMethod("ToString", new Type[0]);
            if (toString != null)
                return (String)toString.Invoke(obj, new Object?[0]);

            Enum? value = obj as Enum;
            if (value != null)
                return value.ToString();
        }

        return type.FullName;
    }
}

////////// ////////// ////////// ////////// //////////

// The base type of the types that are values: ValueType is the base of Struct, Enum is a type of its own.
[Internal]
public class ValueType : Object
{
}

////////// ////////// ////////// ////////// //////////

// The base type of all the structs (the compiler gives it to each struct that does not derive from other).
[Internal]
public class Struct : ValueType
{
}

////////// ////////// ////////// ////////// //////////

[Internal]
public class Enum : ValueType
{
    // The text of the value: the name of its item, the number when the value has no item (a number that is
    // cast to the enum). An enum that declares a ToString keeps its own.
    public virtual String ToString() => Internal.Enum_ToString(this);

    // Parses the text: the name of an item, a number, or the ones that are separated by commas (the flags: they are
    // or-ed). The result is the box of a value of the enum.
    // Throws ArgumentException when the type is not an enum, or the text is not a value of it.
    public static Object Parse(Type enumType, String text, Boolean ignoreCase)
    {
        Object? result = __Parse(enumType, text, ignoreCase);
        if (result == null)
            throw new ArgumentException("the text is not a value of the enum " + enumType.FullName);

        return result;
    }

    public static Object Parse(Type enumType, String text) => Parse(enumType, text, false);

    // Parses the text, returns whether it is a value of the enum.
    public static Boolean TryParse(Type enumType, String text, Boolean ignoreCase, out Object? result)
    {
        result = __Parse(enumType, text, ignoreCase);
        return result != null;
    }

    public static Boolean TryParse(Type enumType, String text, out Object? result)
        => TryParse(enumType, text, false, out result);

    private static Object? __Parse(Type enumType, String text, Boolean ignoreCase)
    {
        if (enumType == null || !enumType.IsEnum || text == null)
            return null;

        return Internal.Enum_Parse(enumType, text, ignoreCase);
    }
}
