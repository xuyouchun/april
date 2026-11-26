// What the garbage collector must keep alive while a lot of garbage is made: an empty array (its address is the end of
// its header), the arrays of structs that hold references and are not a multiple of 8 bytes large (the reference of
// the second element is not aligned), strings in structs, in boxes, in fields and in arrays.

/* EXPECT

empty 0 0 0
objects 100 101 102 3
strings arr-31337 arr-31338 5
solo solo-555 solo-556
box boxP-11 boxS-22 pair-7
fields name-12345 n1-777 inst-4242

*/

import System;

using System;

class Foo { public Int32 V; }

struct PairO { public Foo F; public Int32 B; }

struct PairS { public String A; public Int32 B; }

struct Solo { public String A; }

class Box
{
    public PairS P;
    public Solo S;
}

class Holder
{
    public static String Name;
    public static String[] Names;
    public String Inst;
}

class Program
{
    static Int64 Churn()
    {
        Int64 t = 0;
        for (Int32 i = 0; i < 1500000; i++)
        {
            Foo s = new Foo();
            s.V = i;
            t += s.V;

            Int32[] a = new Int32[3];
            t += a.Length;

            String text = "garbage" + i;
            t += text.Length;
        }

        return t;
    }

    [EntryPoint]
    public static void Main()
    {
        // Arrays that have no elements.
        UInt8[] empty = new UInt8[0];
        Foo[] emptyObjects = new Foo[0];
        String[] emptyStrings = new String[0];

        // Arrays of structs that are 12 bytes large.
        PairO[] objects = new PairO[3];
        for (Int32 k = 0; k < 3; k++)
        {
            objects[k].F = new Foo();
            objects[k].F.V = 100 + k;
            objects[k].B = k;
        }

        PairS[] strings = new PairS[2];
        strings[0].A = "arr-" + 31337;
        strings[1].A = "arr-" + 31338;
        strings[1].B = 5;

        Solo[] solos = new Solo[2];
        solos[0].A = "solo-" + 555;
        solos[1].A = "solo-" + 556;

        Box box = new Box();
        box.P.A = "boxP-" + 11;
        box.S.A = "boxS-" + 22;

        PairS local;
        local.A = "pair-" + 7;
        local.B = 1;
        Object boxed = (Object)local;

        Holder.Name = "name-" + 12345;
        Holder.Names = new String[3];
        Holder.Names[1] = "n1-" + 777;
        Holder holder = new Holder();
        holder.Inst = "inst-" + 4242;

        Churn();

        Console.WriteLine("empty " + empty.Length + " " + emptyObjects.Length + " " + emptyStrings.Length);
        Console.WriteLine("objects " + objects[0].F.V + " " + objects[1].F.V + " " + objects[2].F.V + " " + (objects[0].B + objects[1].B + objects[2].B));
        Console.WriteLine("strings " + strings[0].A + " " + strings[1].A + " " + strings[1].B);
        Console.WriteLine("solo " + solos[0].A + " " + solos[1].A);
        Console.WriteLine("box " + box.P.A + " " + box.S.A + " " + local.A);
        Console.WriteLine("fields " + Holder.Name + " " + Holder.Names[1] + " " + holder.Inst);
    }
}
