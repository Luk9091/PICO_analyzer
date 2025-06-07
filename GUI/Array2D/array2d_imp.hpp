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
    if (_length >= sizeArray){
        if (!isCircular){
            throw std::overflow_error("Array is full");
        } else {
            _length = 0;
        }

    }
    
    _x[_length] = x;
    _y[_length] = y;
    _length++;
    return _length;
}

template <typename T, std::size_t sizeArray>
std::size_t Array2D<T, sizeArray>::append(T y){
    if (_length >= sizeArray){
        if (!isCircular){
            throw std::overflow_error("Array is full");
        } else {
            _length = 0;
        }

    }
    
    _y[_length] = y;
    _length++;
    return _length;
}


template <typename T, std::size_t sizeArray>
void Array2D<T, sizeArray>::clear(){
    _length = 0;
}

template <typename T, std::size_t sizeArray>
void Array2D<T, sizeArray>::set_x_lin(){
    for (uint i = 0 ; i < sizeArray; i++){
        _x[i] = i;
    }
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


template <typename T, std::size_t sizeArray>
T Array2D<T, sizeArray>::end_X(){
    return _x.at(_length);
}

template <typename T, std::size_t sizeArray>
T Array2D<T, sizeArray>::end_Y(){
    if (_length == 0){
        return 0;
    }
    return _y.at(_length - 1);
}


template <typename T, std::size_t sizeArray>
std::span<T>
Array2D<T, sizeArray>::get_X(std::size_t from, std::size_t to){
    auto toRet = std::span<T>(_x.data() + from, to-from);
    return toRet;
}

template <typename T, std::size_t sizeArray>
std::span<T>
Array2D<T, sizeArray>::get_Y(std::size_t from,std::size_t to){
    auto toRet = std::span<T>(_y.data() + from, to-from);
    return toRet;
}



template <typename T, std::size_t sizeArray>
std::size_t
Array2D<T, sizeArray>::length(){
    return _length;
}