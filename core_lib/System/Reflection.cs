

namespace Reflection
{
	////////// ////////// ////////// ////////// //////////

    [__internal__]
	public class ReflectionInfo
	{
		public String Name
		{
			get { return null; }
		}

		public abstract Array<Attribute> GetAttributes();

		public abstract Array<Attribute> GetAttributes(Type attributeType);
	}

	////////// ////////// ////////// ////////// //////////

    [__internal__]
	public sealed class Assembly : ReflectionInfo
	{
		public Array<Type> GetTypes()
		{
			return new Array<Type>(10);
		}
	}


	////////// ////////// ////////// ////////// //////////

    [__internal__]
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

    [__internal__]
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

    [__internal__]
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

    [__internal__]
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

    [__internal__]
	public sealed class Field : Member
	{

	}

	////////// ////////// ////////// ////////// //////////

    [__internal__]
	public sealed class Parameter : ReflectionInfo
	{
		public String Name
		{
			get { return null; }
		}

		public Method Type
		{
			get { return null; }
		}
	}

	////////// ////////// ////////// ////////// //////////

    [__internal__]
	public sealed class GenericParameter : ReflectionInfo
	{
		public String Name
		{
			get { return null; }
		}
	}

	////////// ////////// ////////// ////////// //////////
}


