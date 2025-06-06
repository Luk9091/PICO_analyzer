#pragma once

#include <array>
// #include <pair>
#include <QWidget>


template <typename T, std::size_t sizeArray>
class Array2D{
private:
    std::array <T, sizeArray> _x;
    std::array <T, sizeArray> _y;

    // std::size_t _size size;
    std::size_t _length;


public:
    std::pair<T, T> at(std::size_t index);
    T at_X(std::size_t index);
    T at_Y(std::size_t index);

    std::size_t append(T x, T y);
    void clear();


    std::pair<std::array<T, sizeArray>&, std::array<T, sizeArray>&> get(); 
    std::array<T, sizeArray>& get_X();
    std::array<T, sizeArray>& get_Y();



};