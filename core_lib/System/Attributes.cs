
/*
 *  System.dll
 *
 */

////////// ////////// ////////// ////////// //////////

[Internal]
public class Attribute
{
    // Empty.
}

////////// ////////// ////////// ////////// //////////

[__955825bf_d13a_4d1c_90d5_478e62ceaab6__, AttributeUsage]
public class AttributeUsageAttribute : Attribute
{
    // Empty.
}

////////// ////////// ////////// ////////// //////////

// Core Compile Attribute
[AttributeUsage]
class __955825bf_d13a_4d1c_90d5_478e62ceaab6__ : Attribute
{
    // Empty.
};

[__955825bf_d13a_4d1c_90d5_478e62ceaab6__, AttributeUsage]
class InternalAttribute : Attribute
{
    // Empty.
}

////////// ////////// ////////// ////////// //////////

[__955825bf_d13a_4d1c_90d5_478e62ceaab6__, AttributeUsage]
public class InlineAttribute : Attribute
{
    // Constructor.
    public InlineAttribute()
    {

    }

    // Constructor.
    public InlineAttribute(Boolean value)
    {

    }
}

////////// ////////// ////////// ////////// //////////

[__955825bf_d13a_4d1c_90d5_478e62ceaab6__, AttributeUsage]
public class EntryPointAttribute : Attribute
{
    // Empty.
};

////////// ////////// ////////// ////////// //////////

// The out param can be null when the method returns the value (see the nullable references):
// [MaybeNullWhen(false)] out T value.
[AttributeUsage]
public class MaybeNullWhenAttribute : Attribute
{
    // Constructor.
    public MaybeNullWhenAttribute(Boolean returnValue)
    {

    }
}
