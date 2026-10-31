// Garbage: objects and arrays that nothing reaches are freed, the live ones stay.

/* EXPECT

4549000
5050
5050

*/

import System;

using System;

class Node
{
    public Node(int value, Node next)
    {
        Value = value;
        Next = next;
    }

    public int Value;
    public Node Next;
    public int[] Payload;
}

struct Pair
{
    public Node First;
    public int Number;
}

class Program
{
    static Node Build(int count)
    {
        Node head = null;
        for (int k = 1; k <= count; k++)
            head = new Node(k, head);

        return head;
    }

    static int Sum(Node node)
    {
        int sum = 0;
        while (node != null)
        {
            sum = sum + node.Value;
            node = node.Next;
        }

        return sum;
    }

    [EntryPoint]
    public static void Main()
    {
        // A live list, with arrays hanging from its nodes.
        Node live = Build(100);
        for (Node n = live; n != null; n = n.Next)
        {
            n.Payload = new int[8];
            n.Payload[3] = n.Value;
        }

        // Plenty of garbage: lists, arrays of references and of numbers.
        int total = 0;
        for (int round = 0; round < 2000; round++)
        {
            Node garbage = Build(50);
            Node[] refs = new Node[20];
            refs[7] = garbage;
            int[] numbers = new int[1000];
            numbers[999] = round;

            total = total + Sum(refs[7]) + numbers[999];
        }

        Console.WriteLine(total);

        // The live data is intact.
        int payload = 0;
        for (Node n = live; n != null; n = n.Next)
            payload = payload + n.Payload[3];

        Console.WriteLine(Sum(live));
        Console.WriteLine(payload);
    }
}
