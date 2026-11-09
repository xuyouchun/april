// The language c: a statement with the pointers of C, <%c { ... } %> and ``c statement : int* p, *p, &x, p->f, (T*)p, p[i], p + n,
// sizeof(T), stackalloc T[n]; the variables of the method are the ones of the code of c.

/* EXPECT

types 1 3 2 30
outer 15 20 9
struct 7 8 56 65
copy 1 2 3 4 5
sizeof 4 8 8 1
stack 45 20
string 5 104 111
while 6 15
ptrptr 99 99
single 42
done

*/

import System;

using System;
using System.Runtime;

struct Point
{
    public Int32 X;
    public Int32 Y;
}

class Program
{
    static Int32 Sum(Ptr<Int32> p, Int32 count)
    {
        Int32 total = 0;

        <%c
        {
            for (int* q = p; q < p + count; q++)
                total += *q;
        }
        %>

        return total;
    }

    [EntryPoint]
    public static void Main()
    {
        Int32 x = 5;
        Int32 y = 0;

        // The types, the operators.
        <%c
        {
            int* p = (int*)NativeMemory.Alloc(16);
            p[0] = 1; p[1] = 2; p[2] = 3;
            int* q = p + 2;
            int** pp = &q;
            **pp = 30;
            Console.WriteLine("types " + *p + " " + *(p + 2 - 0) / 10 + " " + (q - p) + " " + p[2]);
        }
        %>

        // The variables of the method are those of the code.
        <%c
        {
            int* px = &x;
            *px += 10;
            int* py = &y;
            *py = *px + 5;
            void* raw = px;
            Console.WriteLine("outer " + *(int*)raw + " " + y + " " + (*(int*)raw - 6));
        }
        %>

        // Structs.
        <%c
        {
            Point* pt = (Point*)NativeMemory.Alloc(16);
            pt->X = 7;
            pt->Y = 8;
            Point* pu = pt + 1;
            pu->X = pt->X * pt->Y;
            pu->Y = pt->X + pt->Y + 50;
            Console.WriteLine("struct " + pt->X + " " + pt->Y + " " + pu->X + " " + pu->Y);
        }
        %>

        // The loops, the copy of the values with *p++ = *q++.
        <%c
        {
            int* src = stackalloc int[5];
            int* dst = stackalloc int[5];
            for (int i = 0; i < 5; i++)
                src[i] = i + 1;

            int* s = src;
            int* d = dst;
            while (s < src + 5)
                *d++ = *s++;

            Console.WriteLine("copy " + dst[0] + " " + dst[1] + " " + dst[2] + " " + dst[3] + " " + dst[4]);
        }
        %>

        // sizeof.
        <%c
        {
            Console.WriteLine("sizeof " + sizeof(int) + " " + sizeof(Point) + " " + sizeof(int*) + " " + sizeof(byte));
        }
        %>

        // stackalloc.
        <%c
        {
            int* values = stackalloc int[10];
            for (int i = 0; i < 10; i++)
                values[i] = i;

            int total = 0;
            for (int i = 0; i < 10; i++)
                total += values[i];

            Console.WriteLine("stack " + total + " " + (values + 5)[0] * 4);
        }
        %>

        // The characters of a string.
        <%c
        {
            String text = "hello";
            char* chars = (char*)Ptr.AddressOf(text);
            Console.WriteLine("string " + text.Length + " " + (int)chars[0] + " " + (int)*(chars + 4));
        }
        %>

        // A call that has the pointer.
        Ptr<Int32> data = NativeMemory.Alloc<Int32>(6);
        for (Int32 i = 0; i < 6; i++)
            data[i] = i;

        Int32 loops = 0;
        <%c
        {
            int* e = data + 6;
            while (e > data)
            {
                e--;
                loops++;
            }
        }
        %>

        Console.WriteLine("while " + loops + " " + Sum(data, 6));

        // A pointer to a pointer.
        Int32 target = 99;
        <%c
        {
            int* a = &target;
            int** b = &a;
            int*** c = &b;
            Console.WriteLine("ptrptr " + **b + " " + ***c);
        }
        %>

        // ``c: one statement.
        Int32 single = 0;
        ``c single = 42;
        Console.WriteLine("single " + single);

        Console.WriteLine("done");
    }
}
