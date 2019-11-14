# C++ Containers

In this practice you are going to study how two common C++ containers work (```std::vector``` and ```std::list```), in relation with memory access performance.

## Question 1
In this first part you will see how ```push_back``` works on both containers.
-  Inserting elements in a vector without pre-allocation.
-  Inserting elements in a vector with memory pre-allocation.
-  Inserting elements in a list.
-  Same as first but using pointers this time.

a) How element insertion differs in a vector and a list?
b) What happens to existing elements in a vector when the container has to grow to store a new element? Are the same after insertion?
c) What happens when memory is pre-allocated?
d) How are elements in a list distributed along memory?
e) What are the differences between using objects and pointer to objects in a container?

## Question 2
Now, let's study how a sorting operation works on each of the containers

a) How are elements inside the container affected? Please briefly describe what's happening under-the-hood.
b) Why syntax is different?

## Question 3
This question will cover speed performance aspects when inserting elements. Please, experiment with different number of elements before making any conclusion.

a) How is the performance between a vector and a list? Which one should be used?
b) Plot the insertion speed for different total number of elements (100, 1000, 5000, 10000, 20000, 100000...)
c) What are the differences between ```C``` and ```D``` classes? How does it affect insertion performance?
d) Repeat the previous plot but using ```D``` elements.
e) Something interesting happened when first executing tests for ```D```. What was it? Why?

## Question 4
This last part inspects speed performance when accessing elements.

a) How is the performance between a vector and a list? Which one should be used?
b) Plot the access speed for different total number of elements (100, 1000, 5000, 10000, 20000, 100000...)

## Side notes
### C class
The only purpose of ```C``` class is to define traceable constructors and destructors, so you can keep track of the life-cycle of the object.

### TicToc class
This helper class will allow you to automatically measure the execution time between the point where the ```TicToc``` object is created and where it is destroyed.
