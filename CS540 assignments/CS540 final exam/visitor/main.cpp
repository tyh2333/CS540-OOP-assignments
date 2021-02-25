/**
 * Fix the code in the .hpp files implementing the visitor pattern
 * (depth-first, left-first).  Your implementation MUST allow the addition of
 * additional Node types, such as DoubleNode, WITHOUT any changes to your Node
 * class.  (The Visitor class may require change, however.) You must also be
 * able to add another visitor type, such as MyVisitor3, without changing any
 * existing code.
 *
 * Hint: You'll need virtual functions and some form of RTTI, such as dynamic_cast or typeid.
 */

// YOUR CODE MUST WORK WITH THIS FILE UNMODIFIED!

#include <iostream>
#include "Visitor.hpp"
//#include "Application.hpp"

/**
 *  right output:
 *
    MyVisitor1: T2
    MyVisitor1: T4
    MyVisitor1: 222
    MyVisitor1: 111
    MyVisitor1: 333
    MyVisitor1: T3
    MyVisitor1: T1

    MyVisitor2: T2
    MyVisitor2: T4
    MyVisitor2: 222
    MyVisitor2: 111
    MyVisitor2: 333
    MyVisitor2: T3
    MyVisitor2: T1
 */
int
main() {
//// Test  1 : TEST Node compile error ++++++++++++++++++

//    TextNode t4{"T4", nullptr, nullptr};
//    TextNode t2{"T2", nullptr, nullptr};
//    IntNode i222{222, nullptr, &t4};
//    IntNode i333{333, nullptr, nullptr};
//    IntNode i111{111, &t2, &i222};
//    TextNode t3{"T3", &i333, nullptr};
//    TextNode t1{"T1", &i111, &t3};



//// Test  2 : TEST Without rescurse ++++++++++++++

    /// these are all Nodes
//    TextNode t4{"T4", nullptr, nullptr};
//    TextNode t2{"T2", nullptr, nullptr};
//    IntNode i222{222, nullptr, &t4};
//
//    IntNode i333{333, nullptr, nullptr};
//    IntNode i111{111, &t2, &i222};
//    TextNode t3{"T3", &i333, nullptr};
//    TextNode t1{"T1", &i111, &t3};

//    MyVisitor1 v0;
//    t1.accept(&v0);
//    t4.accept(v0);
//    i222.accept(v0);
//    i111.accept(v0);
//    i333.accept(v0);
//    t3.accept(v0);
//    t1.accept(v0);

//// Test  3 : Final Visitor check  ++++++++++++++

/// these are all Nodes
    TextNode t4{"T4", nullptr, nullptr};
    TextNode t2{"T2", nullptr, nullptr};
    IntNode i222{222, nullptr, &t4};

    IntNode i333{333, nullptr, nullptr};
    IntNode i111{111, &t2, &i222};
    TextNode t3{"T3", &i333, nullptr};
    TextNode t1{"T1", &i111, &t3};

    MyVisitor1 v1;
    MyVisitor2 v2;
    t1.accept(&v1);
    t1.accept(&v2);
}

