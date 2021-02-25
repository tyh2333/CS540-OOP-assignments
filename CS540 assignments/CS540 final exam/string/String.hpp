//
// Created by macbook on 2020/12/9.
//
/*
 * Implement a string which uses *reference* semantics, as opposed to value
 * semantics.  As usual:
 *     - No memory leaks or other memory errors.
 *     - No warnings when compiled with -Wall -Wextra -pedantic.
 * You are allowed to use a std::string internally, and in fact I suggest that you
 * do that, for ease of implementation.
 * Also, for convenience, put all your code in the header file as inlined functions.
 * You may find the following useful:
 *     std::string::c_str()
 *     strdup()
 * Hint: Internally, you will need to use reference counting.
*/
#ifndef STRING_HPP
#define STRING_HPP
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <utility>

namespace cs540 {
    /// char * strdup( const char *str1 );
    class String {
    public:
        String() : String{""} {}
        String(const String &str) {
            Str_obj = str.Str_obj->inc();
        }
        explicit String(std::string str) {
            Str_obj = new String_obj(std::move(str));
        }
        ~String() {
            Str_obj->dec();
        }
        String &operator=(const String &rhs) {
            if (&rhs != this) {
                Str_obj->dec();
                Str_obj = rhs.Str_obj;
                return *this;
            }
            return *this;
        }
        explicit operator std::string() const {
            return std::string(Str_obj->Str_buf);
        }
        /// overload operator[]
        char &operator[](size_t pos) {
            return Str_obj->Str_buf[pos];
        }
        /// overload operator+
        String operator+(const String &rhs) const {
            return String(std::string(*this) + std::string(rhs));
//            return String(std::string(*this) + std::string(rhs));
        }
        String operator+(const std::string rhs) const {
            return String(std::string(*this) + rhs);
        }
        friend std::ostream &operator<<(std::ostream &stream, const String &rhs) {
            return stream << strdup(rhs.Str_obj->Str_buf);
        }
    private:
        struct String_obj {
            explicit String_obj(std::string str){
                Str_ref_count = 1;
                Str_buf = new char[str.size()+1];
                std::strcpy (Str_buf, str.c_str());
            }
            void dec(){
                if ((--Str_ref_count) == 0)
                    free(Str_buf);
            }
            String_obj* inc() {
                Str_ref_count++;
                return this;
            }
            char *Str_buf;
            int Str_ref_count; /// count how many pointers point to this string, when down to zero, delete
        };
        String_obj *Str_obj;
    };
}
#endif // STRING_HPP
