/*
 * For this problem, you are to implement a statically-sized, 2D array class of
 * ints.
 *
 *     - No memory leaks or other memory errors.
 *     - No warnings when compiled with -Wall -Wextra -pedantic.
 *
 * Put your code in Array2.hpp.
 *
 * This program will print:

Indexing:
1234
1234
567
567
Iteration:
0
1
2
3
4
5
6
7
8
9
10
11
More iteration:
0
1
2
2

 */

/// YOUR CODE MUST WORK WITH THIS FILE UNMODIFIED!

#include "Array2.hpp"
#include <iostream>
int main() {

    Array2<3, 4> a1; /// Creates an 3 by 4 array of ints.  Initialized to zero.
//    Array2 a1(3, 4); // Creates an 3 by 4 array of ints. Initialized to zero.
    std::cout << "Indexing: " << std::endl;
    a1[2][3] = 1234; /// Index operation.
    int &i1(a1[2][3]); /// Bind ref.
    /// Prints 1234 twice.
    std::cout << a1[2][3] << std::endl;
    std::cout << i1 << std::endl;
    i1 = 567; /// Assigns to element.
    /// Prints 567 twice.
    std::cout << a1[2][3] << std::endl;
    std::cout << i1 << std::endl;

    std::cout << "Iteration: " << std::endl;
    /// Iterates in row-major order.
    int i = 0;
    for (auto it = a1.begin(); it != a1.end(); ++it) {
        *it = i++; /// Can assign through iterator.
    }
    /// Prints 0 through 11
    for (auto e : a1) {
        std::cout << e << " " << std::endl;
    }
    std::cout << "More iteration: " << std::endl;
    /// it++ and ++it should work properly.
    auto it = a1.begin();
    std::cout << *it++ << std::endl; /// Should print 0.
    std::cout << *it << std::endl; /// Should print 1.
    std::cout << *++it << std::endl; /// Should print 2.
    std::cout << *it << std::endl; /// Should print 2.
}
