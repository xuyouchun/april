
/*
 *  System.dll
 *
 */

////////// ////////// ////////// ////////// //////////

[Internal]
public struct Void
{

};

////////// ////////// ////////// ////////// //////////

[Internal]
public struct Int8
{
    public new String ToString()
    {
        return Internal.Int64_ToString(this);
    }
};

////////// ////////// ////////// ////////// //////////

[Internal]
public struct UInt8
{
    public new String ToString()
    {
        return Internal.UInt64_ToString(this);
    }
};

////////// ////////// ////////// ////////// //////////

[Internal]
public struct Int16
{
    public new String ToString()
    {
        return Internal.Int64_ToString(this);
    }
};

////////// ////////// ////////// ////////// //////////

[Internal]
public struct UInt16
{
    public new String ToString()
    {
        return Internal.UInt64_ToString(this);
    }
};

////////// ////////// ////////// ////////// //////////

[Internal]
public struct Int32
{
    public new String ToString()
    {
        return Internal.Int64_ToString(this);
    }
};

////////// ////////// ////////// ////////// //////////

[Internal]
public struct UInt32
{
    public new String ToString()
    {
        return Internal.UInt64_ToString(this);
    }
};

////////// ////////// ////////// ////////// //////////

[Internal]
public struct Int64
{
    public new String ToString()
    {
        return Internal.Int64_ToString(this);
    }
};

////////// ////////// ////////// ////////// //////////

[Internal]
public struct UInt64
{
    public new String ToString()
    {
        return Internal.UInt64_ToString(this);
    }
};

////////// ////////// ////////// ////////// //////////

[Internal]
public struct String
{
    public Int32 Length
    {
        get { return Internal.String_GetLength(this); }
    }

    public new String ToString()
    {
        return this;
    }

    public static String Concat(String a, String b)
    {
        return Internal.String_Concat(a, b);
    }

    // Operators: they are found by the name of the operator.
    public static String operator +(String a, String b)
    {
        return Internal.String_Concat(a, b);
    }

    public static String operator +(String a, Int8 b)
    {
        return Internal.String_Concat(a, b.ToString());
    }

    public static String operator +(Int8 a, String b)
    {
        return Internal.String_Concat(a.ToString(), b);
    }

    public static String operator +(String a, UInt8 b)
    {
        return Internal.String_Concat(a, b.ToString());
    }

    public static String operator +(UInt8 a, String b)
    {
        return Internal.String_Concat(a.ToString(), b);
    }

    public static String operator +(String a, Int16 b)
    {
        return Internal.String_Concat(a, b.ToString());
    }

    public static String operator +(Int16 a, String b)
    {
        return Internal.String_Concat(a.ToString(), b);
    }

    public static String operator +(String a, UInt16 b)
    {
        return Internal.String_Concat(a, b.ToString());
    }

    public static String operator +(UInt16 a, String b)
    {
        return Internal.String_Concat(a.ToString(), b);
    }

    public static String operator +(String a, Int32 b)
    {
        return Internal.String_Concat(a, b.ToString());
    }

    public static String operator +(Int32 a, String b)
    {
        return Internal.String_Concat(a.ToString(), b);
    }

    public static String operator +(String a, UInt32 b)
    {
        return Internal.String_Concat(a, b.ToString());
    }

    public static String operator +(UInt32 a, String b)
    {
        return Internal.String_Concat(a.ToString(), b);
    }

    public static String operator +(String a, Int64 b)
    {
        return Internal.String_Concat(a, b.ToString());
    }

    public static String operator +(Int64 a, String b)
    {
        return Internal.String_Concat(a.ToString(), b);
    }

    public static String operator +(String a, UInt64 b)
    {
        return Internal.String_Concat(a, b.ToString());
    }

    public static String operator +(UInt64 a, String b)
    {
        return Internal.String_Concat(a.ToString(), b);
    }

    public static String operator +(String a, Single b)
    {
        return Internal.String_Concat(a, b.ToString());
    }

    public static String operator +(Single a, String b)
    {
        return Internal.String_Concat(a.ToString(), b);
    }

    public static String operator +(String a, Double b)
    {
        return Internal.String_Concat(a, b.ToString());
    }

    public static String operator +(Double a, String b)
    {
        return Internal.String_Concat(a.ToString(), b);
    }

    public static String operator +(String a, Char b)
    {
        return Internal.String_Concat(a, b.ToString());
    }

    public static String operator +(Char a, String b)
    {
        return Internal.String_Concat(a.ToString(), b);
    }

    public static String operator +(String a, Boolean b)
    {
        return Internal.String_Concat(a, b.ToString());
    }

    public static String operator +(Boolean a, String b)
    {
        return Internal.String_Concat(a.ToString(), b);
    }

    // An object is converted by its ToString.
    public static String operator +(String a, Object b)
    {
        if (b == null)
            return Internal.String_Concat(a, null);

        return Internal.String_Concat(a, b.ToString());
    }

    public static String operator +(Object a, String b)
    {
        if (a == null)
            return Internal.String_Concat(null, b);

        return Internal.String_Concat(a.ToString(), b);
    }

    public static Boolean operator ==(String a, String b)
    {
        return Internal.String_Equals(a, b);
    }

    public static Boolean operator !=(String a, String b)
    {
        return !Internal.String_Equals(a, b);
    }

    public static Boolean operator <(String a, String b)
    {
        return Internal.String_Compare(a, b) < 0;
    }

    public static Boolean operator <=(String a, String b)
    {
        return Internal.String_Compare(a, b) <= 0;
    }

    public static Boolean operator >(String a, String b)
    {
        return Internal.String_Compare(a, b) > 0;
    }

    public static Boolean operator >=(String a, String b)
    {
        return Internal.String_Compare(a, b) >= 0;
    }

};

////////// ////////// ////////// ////////// //////////

[Internal]
public struct Boolean
{
    public new String ToString()
    {
        if (this)
            return "True";

        return "False";
    }
};

////////// ////////// ////////// ////////// //////////

[Internal]
public struct Char
{
    public new String ToString()
    {
        return Internal.Char_ToString(this);
    }
};

////////// ////////// ////////// ////////// //////////

[Internal]
public struct Single
{
    public new String ToString()
    {
        return Internal.Single_ToString(this);
    }
};

////////// ////////// ////////// ////////// //////////

[Internal]
public struct Double
{
    public new String ToString()
    {
        return Internal.Double_ToString(this);
    }
};

////////// ////////// ////////// ////////// //////////

[Internal]
public struct Ptr
{

};

////////// ////////// ////////// ////////// //////////


