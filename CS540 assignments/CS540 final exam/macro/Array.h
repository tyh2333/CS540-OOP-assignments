//
// Created by macbook on 2020/12/9.
//

#ifndef FINAL_MACRO_ARRAY_H
#define FINAL_MACRO_ARRAY_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/// type ==> t : int, Person
/**
 * (1) Array_INSTANTIATE:
 *     creates the functions and struct at global scope.
 * It also encodes the length of the array into the struct name.
 * Array_INSTANTIATE(int, 3)
 * @parameter: type t, int num
 */
/// int_3, Person_4 are classes
/// !!!!!! num is also parameter so use t##_##num##_ !!!!!
/// init used here, bc ，t##_init needs t
#define Array_INSTANTIATE(t, num)\
    struct t##_##num{\
        t* data;\
        size_t size;\
        t* (*at) (struct t##_##num *, size_t); \
    };\
    t* t##_##num##_at(struct t##_##num *d, size_t i){\
        if(i >= num){printf("out of range\n"); return 0;}\
        return &(d->data[i]);\
    }\
    void t##_##num##_ctor(struct t##_##num* d){\
        d->data = (t*)malloc(num*sizeof(t));\
        d->at = t##_##num##_at;\
        for(int i = 0; i < num; i++){ t##_init(d->data+i);}\
    }

/// Array_DEFINE defines the struct and also initializes it.
///     Array_DEFINE(MyClass_10, array1):
/// struct MyClass_10 array1;
/// some_ctor_function(&array1);

/// declare int_3, Person_4 a as type T
/// here T means t##_##num, a means array
#define Array_DEFINE(T, a) {T##_ctor(a);}



#endif //FINAL_MACRO_ARRAY_H
