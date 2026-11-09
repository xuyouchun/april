/*
 *  System.dll
 *
 *  Reflection: an event of a type.
 */

namespace Reflection;

////////// ////////// ////////// ////////// //////////

/// <summary>
/// An event of a type.
/// </summary>
[Internal]
public sealed class Event : Member
{
    /// <summary>
    /// Gets the type of the event (the type of its handlers): the same as
    /// <see cref="Member.MemberType"/>.
    /// </summary>
    public Type? EventType => this.MemberType;

    /// <summary>
    /// Gets the method that adds a handler to the event.
    /// </summary>
    public Method? AddMethod => Internal.Reflection_GetEventAdder(this);

    /// <summary>
    /// Gets the method that removes a handler from the event.
    /// </summary>
    public Method? RemoveMethod => Internal.Reflection_GetEventRemover(this);

    /// <summary>
    /// Gets the method that raises the event (the raise accessor), or null if the event cannot be
    /// raised: an event of an interface, an abstract one, and an event that has accessors but no
    /// raise accessor. The events that have no accessors have a raise method that calls their
    /// handlers, if there is any.
    /// </summary>
    public Method? RaiseMethod => Internal.Reflection_GetEventRaiser(this);
}
