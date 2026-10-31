import System;
using System;
using System.Collections.Generic;
/* EXPECT_ERROR Cannot implicitly convert type .IList<Dog>. to .IList<Animal>. */
class Animal { }
class Dog : Animal { }
class Program { [EntryPoint] public static void Main() {
IList<Dog> dogs = new List<Dog>();
IList<Animal> animals = dogs;
} }
