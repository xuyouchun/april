
/*
 *  Types
 *
 */

////////// ////////// ////////// ////////// //////////

[__internal__]
class Object
{

};

////////// ////////// ////////// ////////// //////////

[__internal__]
public class Attribute
{

}

////////// ////////// ////////// ////////// //////////

[__internal__]
class Array : Object
{
    public Int32 Dimension { get { return __dimension; } }

    private Int32 __dimension;

    public Void DoAnything()
    {

    }
};

////////// ////////// ////////// ////////// //////////

[__internal__]
class Array<T> : Array
{
    public Void DoSomething()
    {
        //Console.Write("DoSomething\n");
    }
};

////////// ////////// ////////// ////////// //////////

[__955825bf_d13a_4d1c_90d5_478e62ceaab6__, AttributeUsage]
public class AttributeUsageAttribute : Attribute
{

}

////////// ////////// ////////// ////////// //////////

// Core Compile Attribute
class __955825bf_d13a_4d1c_90d5_478e62ceaab6__
{

};

[__955825bf_d13a_4d1c_90d5_478e62ceaab6__, AttributeUsage]
class __internal__ : Attribute
{

}

////////// ////////// ////////// ////////// //////////

[__955825bf_d13a_4d1c_90d5_478e62ceaab6__, AttributeUsage]
public class InlineAttribute : Attribute
{
    public InlineAttribute()
    {

    }

    public InlineAttribute(Boolean value)
    {

    }
}

////////// ////////// ////////// ////////// //////////

[__955825bf_d13a_4d1c_90d5_478e62ceaab6__, AttributeUsage]
class EntryPointAttribute
{

};




