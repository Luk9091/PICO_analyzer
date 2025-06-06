template <typename T, std::size_t sizeArray>
std::pair<T, T> Array2D<T, sizeArray>::at(std::size_t index){
    if (index >= _length) 
        throw std::out_of_range("Index out og range");
    return {_x[index], _y[index]};
}

template <typename T, std::size_t sizeArray>
T Array2D<T, sizeArray>::at_X(std::size_t index){
    if (index >= _length) 
        throw std::out_of_range("Index out og range");
    return _x[index];
}
template <typename T, std::size_t sizeArray>
T Array2D<T, sizeArray>::at_Y(std::size_t index){
    if (index >= _length) 
        throw std::out_of_range("Index out og range");
    return _y[index];
}


template <typename T, std::size_t sizeArray>
std::size_t Array2D<T, sizeArray>::append(T x, T y){
    if (_length >= sizeArray) 
        throw std::overflow_error("Array is full");
    
    _x[_length] = x;
    _y[_length] = y;
    _length++;
    return _length;
}


template <typename T, std::size_t sizeArray>
void Array2D<T, sizeArray>::clear(){
    _length = 0;
}


template <typename T, std::size_t sizeArray>
std::pair<std::array<T, sizeArray>&, std::array<T, sizeArray>&> 
Array2D<T, sizeArray>::get(){
    return {_x, _y};
}

template <typename T, std::size_t sizeArray>
std::array<T, sizeArray>&
Array2D<T, sizeArray>::get_X(){
    return _x;
}

template <typename T, std::size_t sizeArray>
std::array<T, sizeArray>&
Array2D<T, sizeArray>::get_Y(){
    return _y;
}

// template <typename T, std::size_t sizeArray>
// std::pair<std::array<T, sizeArray>&, std::array<T, sizeArray>&> 
// Array2D<T, sizeArray>::get(){
//     return {_x, _y};
// }

template <typename T, std::size_t sizeArray>
std::span<T>
Array2D<T, sizeArray>::get_X(std::size_t from, std::size_t to){
    if (to > _length) to = _length;
    return std::span<T>(_x.data() + from, to-from);
}

template <typename T, std::size_t sizeArray>
std::span<T>
Array2D<T, sizeArray>::get_Y(std::size_t from,std::size_t to){
    if (to > _length) to = _length;
    return std::span<T>(_y.data() + from, to-from);
}



template <typename T, std::size_t sizeArray>
std::size_t
Array2D<T, sizeArray>::length(){
    return _length;
}