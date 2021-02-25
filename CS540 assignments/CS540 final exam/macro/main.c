/**
correct output
Person 0: id=1, name=Jane
Person 1: id=2, name=Joe
Person 2: id=3, name=Mike
Person 3: id=4, name=Mary
int 0: 1234
int 1: 1234
int 2: 1234
*/
/**
 * Write a macro to create an array with bounds checking.
 * You are only allowed to use C here.
 */

/// YOUR CODE MUST WORK WITH THIS FILE UNMODIFIED!

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "Array.h"
/// Create Array struct containing 3 ints.  Use the int_init() function to
/// initialize elements.
void int_init(int *ip) { *ip = 1234; }
Array_INSTANTIATE(int, 3)
struct Person_struct {
    int id;
    char name[10];
};
typedef struct Person_struct Person;
void Person_init(Person* pp) {
    pp->id = -1;
    pp->name[0] = '\0';
}
/// Create containing Person struct.  Use the Person_init() function to
/// initialize elements.
Array_INSTANTIATE(Person, 4)
int main() {
    /** Test Part
//    int a[2] = {0,1};
//    int_init(a);
//    int_init(a+1);
//    for(int i = 0; i < 2; i++) std::cout<<a[i]<<std::endl;

//    struct int_3 a;
//    int_3_ctor(&a);
//    printf("test .data[idx]\n");
//    printf("%d\n",a.data[0]);
//    printf("%d\n",a.data[1]);
//    printf("%d\n",a.data[2]);
//    printf("test at\n");
//    printf("%d\n",a.at(&a, 0));
//    printf("%d\n",a.at(&a, 1));
//    printf("%d\n",a.at(&a, 2));
//    printf("%d\n",a.at(&a, 3)); /// out of range
//    printf("%d\n",a.size);
     */

    //// Test Person
//    /*
    {
        /// Define an array of 4 Person structs named a.  Note that the size of
        /// the array has been encoded as part of the type name.  Each element
        /// should be initialized by calling the init function for the element
        /// type.
        struct Person_4 a;
        Array_DEFINE(Person_4, &a);
//        Person_4_ctor(&a);
        /// .at() returns a pointer to the given element.  Returns null if out of
        /// range.

        /// Check that init was used.
        assert(a.at(&a, 0)->id == -1);
        assert(a.at(&a, 0)->name[0] == '\0');
        assert(a.at(&a, 3)->id == -1);
        assert(a.at(&a, 3)->name[0] == '\0');
        /// Access members.  Since we are using strict C, we can't use references.
        a.at(&a, 0)->id = 1;
        strcpy(a.at(&a, 0)->name, "Jane");
        a.at(&a, 1)->id = 2;
        strcpy(a.at(&a, 1)->name, "Joe");
        a.at(&a, 2)->id = 3;
        strcpy(a.at(&a, 2)->name, "Mike");
        a.at(&a, 3)->id = 4;
        strcpy(a.at(&a, 3)->name, "Mary");

        /// Out of range should return null pointer.  This will cause a segfault if
        /// dereferenced, but an immediate segfault is 10 times easier to debug than
        /// a memory error.
        assert(a.at(&a, 4) == NULL);

        for (int i = 0; i < 4; i++) {
            printf("Person %d: id=%d, name=%s\n", i, a.at(&a, i)->id, a.at(&a, i)->name);
        }
    }
//*/
    /// Make sure that also works for other types.
    {
        struct int_3 a;
        Array_DEFINE(int_3, &a)
        /// Check that init was used.
        assert(*a.at(&a, 0) == 1234);
        assert(*a.at(&a, 1) == 1234);
        assert(*a.at(&a, 2) == 1234);

        for (int i = 0; i < 3; i++) {
            printf("int %d: %d\n", i, *a.at(&a, i));
        }
        /// Out of range should return null pointer.  This will cause a segfault if
        /// dereferenced, but an immediate segfault is 10 times easier to debug than
        /// a memory error.
        assert(*a.at(&a, 3) == 0);
    }
}
