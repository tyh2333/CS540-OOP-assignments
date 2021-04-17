//
// Created by macbook on 2020/11/19.
//
/**
 * https://en.cppreference.com/w/cpp/memory/shared_ptr
 */
#ifndef CS540_ASS3_SHAREDPTR_HPP
#define CS540_ASS3_SHAREDPTR_HPP
#include <mutex>
#include <iostream>
#include <cstddef> /// std::nullptr_t
namespace cs540{
    std::mutex mutex_540;
    ///+++++++++++++++++++++++++++++++++++++++++++++++++++++++///
    ///+++++++++         Class SharedPtr       +++++++++++++++///
    ///+++++++++++++++++++++++++++++++++++++++++++++++++++++++///

    /** Node class */
    class Node {
    public:
        int reference_count{};
        virtual ~Node() {}
    };

    template <typename U>
    class NodeDerived : public Node {
    public:
        U *data;
        using Node::reference_count;
        NodeDerived(U *tmp) {
            reference_count = 1;
            data = tmp;
        }
        NodeDerived() {
            data = nullptr;
            reference_count = 0;
        }
        ~NodeDerived() {
            delete data;
            data = nullptr;
        }
    };
    /// Introduction:
    /// (1) SharedPtr must allow different smart pointers in different threads
    /// to be safely assigned and unassigned to the same shared objects.
    ///     You may use either locks or atomic increment operations to ensure thread-safety.
    /// You do not need to (and in fact should not) ensure that the same SharedPtr can be used concurrently.

    /// (2) Note that when the smart pointer determines that the object should be deleted,
    /// it must delete the object via a pointer to the original type, even if the template
    /// argument of the final smart pointer is of a base type.
    /// This is to avoid object slicing for non-virtual destructors.

    /// (3) NOTE: We are using the term “object” as in the standard, where it refers to any
    /// region of storage except functions.
    /// Thus, these smart pointers should be able to work with fundamental types as well as
    /// objects of class type.

    /// The smart pointer points to an object of type T.
    /// T may refer to a const-qualified type.
    template<typename T>
    class SharedPtr{
    private:
        T* ptr; /// SharedPtr<T> is similar to T*, so here we have a T* ptr and Node* for obj;
        Node* control_ptr;
    public:
        /// only other SharedPtr can have access to the private attributes:
        template <typename U>
        friend class SharedPtr;

        /// increase operation:
        void inc(){
            if(control_ptr != nullptr){
                std::unique_lock<std::mutex> lock(mutex_540);
                ++control_ptr->reference_count; /// increase use_count
            }
        }
        /// decrease operation:
        void dec(){
            if(control_ptr != nullptr){
                std::unique_lock<std::mutex> lock(mutex_540);
                --control_ptr->reference_count;
                ///     if refer_count down to 0, delete the obj and reset ptr and control_ptr;
                if (control_ptr->reference_count == 0) {
                    delete control_ptr;
                    ptr = nullptr;
                    control_ptr = nullptr;
                }
            }
        }
        ///+++++++++++++++++++++++++++++++++++++++++++++++++++++++///
        /// 1. Constructors, Assignment Operators, and Destructor ///
        ///+++++++++++++++++++++++++++++++++++++++++++++++++++++++///

        /// (1) Constructs a smart pointer that points to null.
        SharedPtr(){
            this->ptr = nullptr;
            this->control_ptr = nullptr;
        }

        /// (2) Constructs a smart pointer that points to the given object.
        /// The reference count is initialized to one.
        template <typename U>
        explicit SharedPtr(U *data){
            this->ptr = data;
            /// reference count operation will be done in NodeDerived(U*)
            this->control_ptr = new NodeDerived<U>(data);
        }
        /// (3) If p is not null, then reference count of the managed object is incremented.
        SharedPtr(const SharedPtr &p){
            this->ptr = p.ptr;
            this->control_ptr = p.control_ptr;
            if(ptr != nullptr){
                inc();
            }
        }
        /// If U * is not implicitly convertible to T *, use of the second: (SharedPtr(const SharedPtr &p))
        /// constructor will result in a compile-time error when the compiler attempts to
        /// instantiate the member template.
        template <typename U>
        SharedPtr(const SharedPtr<U> &p){
            this->ptr = p.ptr;
            this->control_ptr = p.control_ptr;
            if(ptr != nullptr){
                inc();
            }
        }

        /// (4) Move the managed object from the given smart pointer.
        /// The reference count must remain unchanged. After this function, p must be null.
        /// This must work if U * is implicitly convertible to T *.
        SharedPtr(SharedPtr &&p){
            /// move copy, transfer control from p to *this
            this->ptr = p.ptr;
            this->control_ptr = p.control_ptr;
            /// After this function, p must be null.
            p.ptr = nullptr;
            p.control_ptr = nullptr;
        }

        template <typename U>
        SharedPtr(SharedPtr<U> &&p){
            /// move copy, transfer control from p to *this
            this->ptr = p.ptr;
            this->control_ptr = p.control_ptr;
            /// After this function, p must be null.
            p.ptr = nullptr;
            p.control_ptr = nullptr;
        }

        /// (5) Copy assignment.
        /// Must handle self assignment.
        /// Decrement reference count of current object, if any,
        /// and increment reference count of the given object.
        /// If U * is not implicitly convertible to T *, this will result in a syntax error.
        /// Note that both the normal assignment operator and a member
        /// template assignment operator must be provided for proper operation.
        SharedPtr &operator=(const SharedPtr &p){
            /// Must handle self assignment.
            if(*this != p){
                /// Decrement reference count of current object, if any:
                /// *this reference count need to -1, so use dec();
                this->dec();
                this->ptr = p.ptr;
                this->control_ptr = p.control_ptr;
                /// Increment reference count of the given object.
                /// now *this has the control of p, so just use inc();
                this->inc();
            }
            return *this;
        }

        template <typename U>
        SharedPtr<T> &operator=(const SharedPtr<U> &p){
            /// Must handle self assignment.
            if(*this != p){
                /// Decrement reference count of current object, if any:
                this->dec();
                this->ptr = p.ptr;
                this->control_ptr = p.control_ptr;
                /// Increment reference count of the given object.
                this->inc();
            }
            return *this;
        }

        /// (6) Move assignment.
        /// After this operation, p must be null.
        /// The reference count associated with the object
        /// (if p is not null before the operation) will remain the same after this operation.
        /// This must compile and run correctly if U * is implicitly convertible to T *,
        /// otherwise, it must be a syntax error.

        SharedPtr &operator=(SharedPtr &&p){
            /// Must handle self assignment.
            if(*this != p){
                /// Decrement reference count of current object,so dec();
                this->dec();
                /// *this get teh control of p, and delete p, bcof move copy
                this->ptr = p.ptr;
                this->control_ptr = p.control_ptr;
                p.ptr = nullptr;
                p.control_ptr = nullptr;
                /// no need for inc(); bcof move copy doesn't increase reference count;
            }
            return *this;
        }

        template <typename U>
        SharedPtr &operator=(SharedPtr<U> &&p){
            /// Must handle self assignment.
            if(*this != p){
                /// Decrement reference count of current object,so dec();
                this->dec();
                /// *this get teh control of p, and delete p, bcof move copy;
                this->ptr = p.ptr;
                this->control_ptr = p.control_ptr;
                p.ptr = nullptr;
                p.control_ptr = nullptr;
                /// no need for inc(); bcof move copy doesn't increase reference count;
            }
            return *this;
        }

        /// (7) Decrement reference count of managed object.
        /// If the reference count is zero, delete the object.
        ~SharedPtr(){
            /// when destruction, only dec() once, bc other SharedPtr may still has control of current obj;
            dec();
        }

        /// (8) Convert sp by using static_cast and dynamic_cast to cast the contained pointer.
        /// It will result in a syntax error if static_cast or dynamic_cast cannot be applied
        /// to the relevant types, and will result in a smart pointer to null
        /// if the dynamic type of the pointer in sp is not T *.

        /// (i)   static_cast (SharedPtr<U>) to  SharedPtr<T>
        /// (ii) dynamic_cast (SharedPtr<U>) to  SharedPtr<T>
        template <typename U>
        SharedPtr(const SharedPtr<U> &sp, T *data){
            ptr = data;
            control_ptr = sp.control_ptr;
            /// if convert successfully: inc()
            if (ptr) inc();
            /// else : result in a smart pointer to nullptr
            else control_ptr = nullptr;
        }
        ///+++++++++++++++++++++++++++++++++++++++++++++++++++++++///
        ///+++++++++         2.   Modifiers        +++++++++++++++///
        ///+++++++++++++++++++++++++++++++++++++++++++++++++++++++///

        /// (8) The smart pointer is set to point to the null pointer.
        /// The reference count for the currently pointed to object,
        /// if any, is decremented.
        void reset(){
            dec();
            ptr = nullptr;
            control_ptr = nullptr;
        }

        /// (9) Replace owned resource with another pointer.
        /// If the owned resource has no other references, it is deleted.
        /// If p has been associated with some other smart pointer, the behavior is undefined.
        template <typename U>
        void reset(U *p){
            dec();
            ptr = p;
            /// Replace owned resource with another pointer.
            control_ptr = new NodeDerived<U>(p);
        }

        ///+++++++++++++++++++++++++++++++++++++++++++++++++++++++///
        ///+++++++++         3.   Observers        +++++++++++++++///
        ///+++++++++++++++++++++++++++++++++++++++++++++++++++++++///

        /// (10) Returns a pointer to the owned object.
        /// Note that this will be a pointer-to-const if T is a const-qualified type.
        /// @return The stored pointer, i.e., get()
        T *get() const{ return ptr; }

        /// (11) A reference to the pointed-to object is returned.
        /// Note that this will be a const-reference if T is a const-qualified type.
        /// @return The result of dereferencing the stored pointer, i.e., *get()
        T &operator*() const{ return *ptr;}

        /// (12) The pointer is returned.
        /// Note that this will be a pointer-to-const if T is a const-qualified type.
        /// @return The stored pointer, i.e., get()
        T *operator->() const{ return ptr;}

        /// (13) Returns true if the SharedPtr is not null.
        explicit operator bool() const{ return (ptr != nullptr); }
    };
    ///+++++++++++++++++++++++++++++++++++++++++++++++++++++++///
    ///+++++++++         Free functions        +++++++++++++++///
    ///+++++++++++++++++++++++++++++++++++++++++++++++++++++++///

    /// (1) Returns true if the two smart pointers point to the same object
    /// or if they are both null. Note that implicit conversions may be applied.
    template <typename T1, typename T2>
    bool operator==(const SharedPtr<T1> &lhs, const SharedPtr<T2> &rhs){
        return(lhs.get() == rhs.get());
    }

    /// (2) Compare the SharedPtr against nullptr.
    template <typename T>
    bool operator==(const SharedPtr<T> &lhs, std::nullptr_t){
        return !lhs;
    }

    template <typename T>
    bool operator==(std::nullptr_t, const SharedPtr<T> &rhs){
        return !rhs;
    }

    /// (3) True if the SharedPtrs point to different objects,
    /// or one points to null while the other does not.
    /// Just use the opposite of the overloaded operation==
    template <typename T1, typename T2>
    bool operator !=(const SharedPtr<T1> &lhs, const SharedPtr<T2> &rhs){
        return !(lhs == rhs);
    }

    /// (4) Compare the SharedPtr against nullptr.
    template <typename T>
    bool operator!=(const SharedPtr<T> &lhs, std::nullptr_t){
        return (bool)lhs;
    }

    template <typename T>
    bool operator!=(std::nullptr_t, const SharedPtr<T> &rhs){
        return (bool)rhs;
    }

    /// (5) Convert sp by using static_cast to cast the contained pointer.
    /// It will result in a syntax error if static_cast cannot be applied
    /// to the relevant types.

    /// r - The pointer to convert
    /// Y: typename SharedPtr<T>::element_type
    /// return static_cast<Y*>(r.get())
    template <typename T, typename U>
    SharedPtr<T> static_pointer_cast(const SharedPtr<U> &sp){
        auto p = static_cast<T*>(sp.get());
        return SharedPtr<T>(sp, p);
    }

    /// (6) Convert sp by using dynamic_cast to cast the contained pointer.
    /// It will result in a syntax error if dynamic_cast cannot be applied
    /// to the relevant types, and will result in a smart pointer to null
    /// if the dynamic type of the pointer in sp is not T *.

    /// r - The pointer to convert
    /// Y: typename SharedPtr<T>::element_type
    /// return dynamic_cast<Y*>(r.get())
    template <typename T, typename U>
    SharedPtr<T> dynamic_pointer_cast(const SharedPtr<U> &sp){
        auto p = dynamic_cast<T*>(sp.get());
        return SharedPtr<T>(sp, p);
    }
}
#endif //CS540_ASS3_SHAREDPTR_HPP
