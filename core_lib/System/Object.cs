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

        // An enum: its ToString (made by the compiler) gives the name of the item.
        Type type = Type.GetType(obj);
        if (type.IsEnum)
        {
            Method? toString = type.GetMethod("ToString", new Type[0]);
            if (toString != null)
                return (String)toString.Invoke(obj, new Object?[0]);
        }

        return type.FullName;
    }
}

////////// ////////// ////////// ////////// //////////

[Internal]
public class Enum
{
    // The text of a value of an enum that has no item (the ToString of the enums, made by the compiler): its number.
    public static String __Number(Int64 value) => "" + value;
}
