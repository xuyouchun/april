
import System;

using System;

class Project1
{
    [EntryPoint]
    public static void Main()
    {
        Class1 obj1 = new Class1(100);
        Class1 obj2 = new Class1(200);
        Class1 obj3 = new Class1(300);
        Class1 obj4 = new Class1(400);

        int v = ((obj1 + obj2) * 2).Value;

        Console.Print(v);
    }

    class Class1
    {
        public Class1(int value)
        {
            this.Value = value;
        }

        public int Value { get; set; }

        public static Class1 operator + (Class1 obj1, Class1 obj2)
        {
            return new Class1(obj1.Value + obj2.Value);
        }

        public static Class1 operator - (Class1 obj1, Class1 obj2)
        {
            return new Class1(obj1.Value - obj2.Value);
        }

        public static Class1 operator * (Class1 obj1, int num)
        {
            return new Class1(obj1.Value * num);
        }
    };
};


