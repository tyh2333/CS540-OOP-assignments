//
// Created by macbook on 2020/9/12.
//

#ifndef TEST_DEQUE_H
#define TEST_DEQUE_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>

#define Deque_DEFINE(t) \
    struct Deque_##t##_Iterator; \
    struct Deque_##t {  \
        /*variables*/\
        t *array; \
        size_t in; /*in:  push_front and pop_front here*/\
        size_t out;/*out: push_back  and pop_back  here*/\
        size_t capacity; /* the capacity of the deque */ \
        size_t d_size;   /* the idx size of the deque */ \
        char type_name[sizeof("Deque_"#t)] = "Deque_"#t; \
        /*functions*/\
        size_t (*size)(Deque_##t *);\
        bool (*empty)(Deque_##t *); \
        t &(*front)(Deque_##t *);\
        t &(*back)(Deque_##t *); \
        void double_capacity(Deque_##t *d);  \
        void (*push_back)(Deque_##t *, t value);\
        void (*push_front)(Deque_##t *, t value);  \
        void (*pop_front)(Deque_##t *);\
        void (*pop_back)(Deque_##t *); \
        t &(*at)(Deque_##t *, size_t i);  \
        void (*clear)(Deque_##t *); \
        bool (*equal)(Deque_##t &, Deque_##t &);\
        void (*dtor)(Deque_##t *);  \
        Deque_##t##_Iterator (*begin)(Deque_##t *);\
        Deque_##t##_Iterator (*end)(Deque_##t *);  \
        bool (*cmp)(const t &, const t &);\
        void (*sort)(Deque_##t *,Deque_##t##_Iterator,Deque_##t##_Iterator);\
    };\
    struct Deque_##t##_Iterator{ \
        size_t idx;\
        Deque_##t * deq; \
        t &(*deref)(Deque_##t##_Iterator *); \
        void (*inc)(Deque_##t##_Iterator *); \
        void (*dec)(Deque_##t##_Iterator *); \
        bool (*equal)(const Deque_##t##_Iterator &, const Deque_##t##_Iterator &);  \
    };\
    void Deque_##t##_sort(Deque_##t *d, Deque_##t##_Iterator it1, Deque_##t##_Iterator it2){\
		std::sort(&d->array[it1.idx], &d->array[it2.idx], d->cmp);/*must use std::*/\
	}\
    void Deque_##t##_Iterator_inc(Deque_##t##_Iterator *it){	 \
		it->idx=(it->idx+1) % (it->deq->capacity);	 \
	}	 \
	void Deque_##t##_Iterator_dec(Deque_##t##_Iterator *it){	 \
		it->idx=(it->idx+it->deq->capacity-1) % (it->deq->capacity);	 \
	}	 \
	t & Deque_##t##_Iterator_deref(Deque_##t##_Iterator *it){	 \
		return it->deq->array[it->idx]; \
	}	 \
    Deque_##t##_Iterator Deque_##t##_begin(Deque_##t *deq){ \
		Deque_##t##_Iterator it; \
		it.idx=deq->in;	 \
		it.deq = deq; \
		it.inc = &Deque_##t##_Iterator_inc; \
		it.dec= & Deque_##t##_Iterator_dec; \
		it.deref= & Deque_##t##_Iterator_deref;	 \
		return it; \
	}	 \
	Deque_##t##_Iterator Deque_##t##_end(Deque_##t *deq){ \
		Deque_##t##_Iterator it; \
		it.idx=(deq->out+1)%deq->capacity;	 \
		it.deq = deq; \
		it.inc = &Deque_##t##_Iterator_inc; \
		it.dec=  &Deque_##t##_Iterator_dec; \
		it.deref= & Deque_##t##_Iterator_deref;	 \
		return it;	 \
	}	 \
	bool Deque_##t##_Iterator_equal(Deque_##t##_Iterator it1, Deque_##t##_Iterator it2){	\
		return it1.idx == it2.idx;  \
	}	 \
    bool Deque_##t##_equal(Deque_##t &d1, Deque_##t &d2){\
        if(d1.d_size == d2.d_size){ \
            for(size_t i = 0; i < d1.d_size; i++){ \
                if(d1.cmp(d1.at(&d1, i), d2.at(&d2,i)) \
                || d2.cmp(d2.at(&d2, i), d1.at(&d1, i))){ \
                    return false; /*if d1=d2, condition will be false */\
                } \
            } return true;\
        }\
        return false;\
    } \
    size_t Deque_##t##_size(Deque_##t *d){\
        return d->d_size;  \
    } \
    bool Deque_##t##_empty(Deque_##t *d){ \
        return d->d_size == 0;\
    } \
    t &Deque_##t##_front(Deque_##t *d){\
        return d->array[d->in];  \
    } \
    t &Deque_##t##_back(Deque_##t *d){ \
        return d->array[d->out]; \
    } \
    t &Deque_##t##_at(Deque_##t *d, size_t i){  \
        if((i >= 0) && (i < d->d_size)){  \
            return d->array[(d->in + i)%d->capacity]; \
        }\
    } \
    void Deque_##t##_clear(Deque_##t *d){ \
        d->size = 0; \
        d->in = 0;\
        d->out = 0;  \
        memset(d->array, 0, d->capacity * sizeof(t)); \
    } \
    void double_capacity(Deque_##t *d){ \
    /*when needs to expand, malloc 2*capacity and copy the original one */\
		t *temp = (t*) malloc(sizeof(t) * 2 * d->capacity); \
		for(size_t i=0; i < d->capacity; i++){	 \
			size_t loc=(d->in + i) % (d->capacity);	 \
			temp[i] = d->array[loc] ;	}								  \
		free(d->array); \
		d->array = temp;												  \
		d->in=0;	 \
		d->out = d->capacity; \
		d->capacity = 2 * d->capacity; \
	} \
    void Deque_##t##_push_back(Deque_##t *d, t value){  \
        if(d->d_size == d->capacity) double_capacity(d);\
        else if(d->d_size > 0) d->out = (d->out+1)%d->capacity;/* d_size > 0*/  \
        d->array[d->out] = value; /*we put the value in d->out*/ \
        d->d_size++; /*increase idx size of deque*/ \
    }\
    void Deque_##t##_push_front(Deque_##t *d, t value){ \
        if(d->d_size == d->capacity) double_capacity(d);\
        else  d->in = (d->in+d->capacity - 1)%d->capacity; \
        d->array[d->in] = value;\
        d->d_size++;\
    }\
    void Deque_##t##_pop_front(Deque_##t *d){  \
        if(d->d_size > 0){\
           d->d_size--;\
           d->in = (d->in + 1)% d->capacity;\
        }else printf("!!!bug info: no value no pop!\n");\
    }\
    void Deque_##t##_pop_back(Deque_##t *d){\
        if(d->d_size > 0){/*can only pop when have values to pop*/  \
            d->d_size--;  \
            d->out = (d->out + d->capacity - 1)% d->capacity; \
        }else printf("!!!bug info: no value no pop!\n");\
    }\
    void Deque_##t##_dtor(Deque_##t *d){ \
        free(d->array);\
        d = nullptr;\
    }\
    void Deque_##t##_ctor(Deque_##t *d,  \
            bool (*less)(const t &, const t &)) { \
        d->array = (t*)malloc(10 * sizeof(t)); \
        d->in = 0;  \
        d->out = 0; \
        d->d_size = 0; \
        d->capacity = 10; \
        strcpy(d->type_name, "Deque_" #t);  \
        d->size = &Deque_##t##_size;  \
        d->empty = &Deque_##t##_empty;\
        d->at = &Deque_##t##_at;\
        d->clear = &Deque_##t##_clear;\
        d->front = &Deque_##t##_front;\
        d->back = &Deque_##t##_back;  \
        d->push_back = &Deque_##t##_push_back; \
        d->push_front = &Deque_##t##_push_front;  \
        d->pop_front = &Deque_##t##_pop_front; \
        d->pop_back = &Deque_##t##_pop_back;\
        d->dtor = &Deque_##t##_dtor;  \
        d->equal = &Deque_##t##_equal; \
        d->begin = &Deque_##t##_begin; \
        d->end = &Deque_##t##_end;  \
        d->cmp = less;\
        d->sort = &Deque_##t##_sort;\
}

#endif //TEST_DEQUE_H */