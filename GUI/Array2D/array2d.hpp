#pragma once

#include <array>
#include <span>
// #include <pair>
#include <QWidget>


template <typename T, std::size_t sizeArray>
class Array2D{
private:
    std::array <T, sizeArray> _x {0};
    std::array <T, sizeArray> _y {0};

    // std::size_t _size size;
    std::size_t _length = 0;
    bool isCircular = true;


public:
    std::pair<T, T> at(std::size_t index);
    T at_X(std::size_t index);
    T at_Y(std::size_t index);

    std::size_t append(T x, T y);
    std::size_t append(T y);
    void clear();
    void set_x_lin();


    std::pair<std::array<T, sizeArray>&, std::array<T, sizeArray>&> get(); 
    std::array<T, sizeArray>& get_X();
    std::array<T, sizeArray>& get_Y();

    T end_X();
    T end_Y();
    
    std::span<T> get_X(std::size_t from, std::size_t to);
    std::span<T> get_Y(std::size_t from, std::size_t to);

    std::size_t length();

};


#include "array2d_imp.hpp"