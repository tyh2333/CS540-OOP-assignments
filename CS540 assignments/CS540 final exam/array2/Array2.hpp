//
// Created by macbook on 2020/12/9.
//
#ifndef ARRAY2_HPP
#define ARRAY2_HPP
#include <vector>
#include <string>
template<int R, int C> /// Array2<3,4> 3 means row, 4 means col;
struct Array2 {
    /// Use one dimensional std:::vector to store all the data;
    /// So we can use iterators of std::vector, like begin(), end(), ++
    std::vector<int> data;
    int rows, cols;
    struct Sub_Array2 {
        Array2 &sub_data;
        int idx;
        Sub_Array2(Array2 &tmp_data, int tmp_idx) : sub_data(tmp_data), idx(tmp_idx) {}
        /// For [][]
        int &operator[](const int &tmp_idx) { return sub_data.get(tmp_idx);}
    };
    /// For [][]
    Sub_Array2 operator[](const int &idx) { return Sub_Array2(*this, idx);}
    Array2<R,C>(){
        rows = R;
        cols = C;
        data = std::vector<int>(rows * cols);
    }
    std::vector<int>::iterator begin() { return data.begin(); }
    std::vector<int>::iterator end() { return data.end(); }
    int &get(int cur_idx) { return data[cur_idx]; }
};
#endif //ARRAY2_HPP
