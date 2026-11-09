// GetAttributes of the members and of the types: the attributes are made with the constructor and
// the arguments that were written (strings, numbers, a bool, an enum), in the order that they were
// written; GetAttributes(type) keeps the ones of a type (or of a type that is derived from it);
// Method.NewInstance makes an object with a constructor.

/* EXPECT

type 2 ann 3
field bob 5 True
method 1 run
property ok
parameter x
none 0
filter 1 ann
base 2
derived 1
instance 7 seven
info Doc 2
generic Pair<Int32> 7
named text 3 note True 2.5
named type 9 none
assembly 2 asm ann
done

*/

import System;

using System;
using System.Reflection;

[assembly: Author("asm", 1)]
[assembly: Label("ann", Level = 4)]

enum Level { Low, High }

class AuthorAttribute : Attribute
{
    public String Name;
    public Int32 Count;

    public AuthorAttribute(String name, Int32 count) { Name = name; Count = count; }
}

class FlagAttribute : Attribute
{
    public Boolean On;

    public FlagAttribute(Boolean on) { On = on; }
}

class NoteAttribute : Attribute
{
    public String Text;
    public Level Level;

    public NoteAttribute(String text, Level level) { Text = text; Level = level; }
}

class TaggedAttribute : Attribute
{
    public Int32 Value;

    public TaggedAttribute(Int32 value) { Value = value; }
}

class SpecialTaggedAttribute : TaggedAttribute
{
    public SpecialTaggedAttribute(Int32 value) : base(value) { }
}

[Author("ann", 3)]
[Flag(true)]
class Doc
{
    [Author("bob", 5)]
    [Flag(true)]
    public Int32 Count;

    public Int32 Plain;

    [Tagged(1)]
    public void Run([Author("x", 0)] Int32 x) { }

    [Note("run", Level.High)]
    public Int32 Value { get { return 0; } }
}

class Tagged
{
    [Tagged(1)]
    [SpecialTagged(2)]
    public Int32 Mixed;
}

class LabelAttribute : Attribute
{
    public String Text;
    public Int32 Level;
    public Boolean Strong;
    public Double Weight;
    public String Note { get; set; }

    public LabelAttribute(String text) { Text = text; }
}

[Label("type", Level = 9)]
class Labeled
{
    [Label("text", Level = 3, Note = "note", Strong = true, Weight = 2.5)]
    public Int32 Field;
}

class Seven
{
    public Int32 Number;
    public String Name;

    public Seven(Int32 number, String name) { Number = number; Name = name; }
}

class Pair<[Tagged(7)] T>
{
    public T First;
}

class Program
{
    [EntryPoint]
    public static void Main()
    {
        Type type = Type.GetType(new Doc());

        Array<Attribute> attributes = type.GetAttributes();
        AuthorAttribute author = (AuthorAttribute)attributes[0];
        Console.WriteLine("type " + attributes.Length + " " + author.Name + " " + author.Count);

        Array<Attribute> field = type.GetField("Count").GetAttributes();
        AuthorAttribute fieldAuthor = (AuthorAttribute)field[0];
        FlagAttribute fieldFlag = (FlagAttribute)field[1];
        Console.WriteLine("field " + fieldAuthor.Name + " " + fieldAuthor.Count + " " + fieldFlag.On);

        Method run = type.GetMethod("Run", new Type[] { Type.GetType(1) });
        Array<Attribute> methods = run.GetAttributes();
        TaggedAttribute tagged = (TaggedAttribute)methods[0];
        Console.WriteLine("method " + methods.Length + " run");

        Array<Attribute> property = type.GetProperty("Value").GetAttributes();
        NoteAttribute note = (NoteAttribute)property[0];
        Console.WriteLine("property " + (note.Level == Level.High && note.Text == "run"? "ok" : "wrong"));

        Array<Attribute> parameter = run.GetParameters()[0].GetAttributes();
        AuthorAttribute parameterAuthor = (AuthorAttribute)parameter[0];
        Console.WriteLine("parameter " + parameterAuthor.Name);

        Console.WriteLine("none " + type.GetField("Plain").GetAttributes().Length);

        Array<Attribute> filtered = type.GetAttributes(Type.GetType(new AuthorAttribute("", 0)));
        Console.WriteLine("filter " + filtered.Length + " " + ((AuthorAttribute)filtered[0]).Name);

        Type mixed = Type.GetType(new Tagged());
        Array<Attribute> mixedAttributes = mixed.GetField("Mixed").GetAttributes();
        Console.WriteLine("base " + mixedAttributes.Length);
        Console.WriteLine("derived " + mixed.GetField("Mixed").GetAttributes(
                                    Type.GetType(new SpecialTaggedAttribute(0))).Length);

        Type seven = Type.GetType(new Seven(0, ""));
        Object?[] args = new Object?[2];
        args[0] = 7;
        args[1] = "seven";
        Seven made = (Seven)seven.GetMethods()[0].NewInstance(args);
        Console.WriteLine("instance " + made.Number + " " + made.Name);

        // A type is an object of reflection too.
        ReflectionInfo info = type;
        Console.WriteLine("info " + info.Name + " " + info.GetAttributes().Length);

        Type pair = Type.GetType(new Pair<Int32>());
        TaggedAttribute generic = (TaggedAttribute)pair.GetGenericParameters()[0].GetAttributes()[0];
        Console.WriteLine("generic " + pair.Name + " " + generic.Value);

        // The named arguments assign the fields and the properties after the constructor.
        Type labeled = Type.GetType(new Labeled());
        LabelAttribute label = (LabelAttribute)labeled.GetField("Field").GetAttributes()[0];
        Console.WriteLine("named " + label.Text + " " + label.Level + " " + label.Note + " "
                          + label.Strong + " " + label.Weight);

        LabelAttribute typeLabel = (LabelAttribute)labeled.GetAttributes()[0];
        Console.WriteLine("named " + typeLabel.Text + " " + typeLabel.Level + " "
                          + (typeLabel.Note == null? "none" : typeLabel.Note));

        // The attributes of the assembly: [assembly: X].
        Array<Attribute> assembly = type.Assembly.GetAttributes();
        Console.WriteLine("assembly " + assembly.Length + " " + ((AuthorAttribute)assembly[0]).Name
                          + " " + ((LabelAttribute)assembly[1]).Text);

        Console.WriteLine("done");
    }
}
