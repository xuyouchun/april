

namespace Reflection
{
	////////// ////////// ////////// ////////// //////////

    [Internal]
	public abstract class ReflectionInfo
	{
		public String Name
		{
			get { return null; }
		}

		public abstract Array<Attribute> GetAttributes();

		public abstract Array<Attribute> GetAttributes(Type attributeType);
	}

	////////// ////////// ////////// ////////// //////////

    [Internal]
	public sealed class Assembly : ReflectionInfo
	{
		public Array<Type> GetTypes()
		{
			return new Array<Type>(10);
		}
	}


	////////// ////////// ////////// ////////// //////////

    [Internal]
	public class Member : ReflectionInfo
	{
		public Type Type
		{
			get { return null; }
		}

		public Type DeclaringType
		{
			get { return null; }
		}
	}

	////////// ////////// ////////// ////////// //////////

    [Internal]
	public sealed class Method : Member
	{
		public Array<Parameter> GetParameters()
		{
			return new Array<Parameter>(10);
		}

		public Array<GenericParameter> GetGenericParameters()
		{
			return new Array<GenericParameter>(10);
		}
	}

	////////// ////////// ////////// ////////// //////////

    [Internal]
	public sealed class Property : Member
	{
		public Method GetMethod
		{
			get { return null; }
		}

		public Method SetMethod
		{
			get { return null; }
		}
	}

	////////// ////////// ////////// ////////// //////////

    [Internal]
	public sealed class Event : Member
	{
		public Method AddMethod
		{
			get { return null; }
		}

		public Method RemoveMethod
		{
			get { return null; }
		}
	}

	////////// ////////// ////////// ////////// //////////

    [Internal]
	public sealed class Field : Member
	{

	}

	////////// ////////// ////////// ////////// //////////

    [Internal]
	public sealed class Parameter : ReflectionInfo
	{
		public Method Type
		{
			get { return null; }
		}
	}

	////////// ////////// ////////// ////////// //////////

    [Internal]
	public sealed class GenericParameter : ReflectionInfo
	{

	}

	////////// ////////// ////////// ////////// //////////
}


