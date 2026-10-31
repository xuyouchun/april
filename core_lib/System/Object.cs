/*
 *  System.dll
 *
 *  Object, Type and Enum.
 */

using Reflection;

////////// ////////// ////////// ////////// //////////

[Internal]
public class Object
{
    // Returns the object type.
    public Type GetType()
    {
        return Internal.Reflection_GetType(this);
    }

    // Returns the text of the object: the name of its type, unless the type overrides it.
    public virtual String ToString()
    {
        return GetType().Name;
    }
}

////////// ////////// ////////// ////////// //////////

[Internal]
public sealed class Type
{
    public String Name
    {
        get { return Internal.Reflection_GetTypeName(this); }
    }

    public Array<Field> GetFields()
    {
        return Array.New<Field>(10);
    }

    public Field GetField(String name)
    {
        return null;
    }

    public Array<Method> GetMethods()
    {
        return Array.New<Method>(10);
    }

    public Array<Method> GetMethods(String name)
    {
        return Array.New<Method>(10);
    }

    public Method GetMethod(String name, Array<Type> parameterTypes)
    {
        return null;
    }

    public Array<Property> GetProperties()
    {
        return Array.New<Property>(10);
    }

    public Property GetProperty(String name)
    {
        return null;
    }

    public Array<Event> GetEvents()
    {
        return Array.New<Event>(10);
    }

    public Event GetEvent(String name)
    {
        return null;
    }

    public Array<GenericParameter> GetGenericParameters()
    {
        return Array.New<GenericParameter>(10);
    }

    public Assembly Assembly
    {
        get { return null; }
    }
}

////////// ////////// ////////// ////////// //////////

[Internal]
public class Enum
{

}
