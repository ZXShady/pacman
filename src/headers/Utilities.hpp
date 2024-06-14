#ifndef PACMAN_UTILITIES_HPP
#define PACMAN_UTILITIES_HPP

#include <cstdint>
#include <type_traits>

namespace pacman {

template<typename T>
using in = typename std::conditional<(sizeof(T) < sizeof(void*) * 2 && std::is_trivially_copyable<T>::value), T, const T&>::type;


template<typename T,std::size_t N>
constexpr std::size_t size(T(&)[N]) 
{
    return N;
}

template<typename T>
constexpr std::size_t size(T& t) 
{
    return t.size();
}

template<typename Container, typename Value>
bool contains(const Container& t,const Value& value) noexcept
{
    for (const auto& v : t)
        if (v == value)
            return true;
    return false;
}

}

#endif // !defined(PACMAN_UTILITIES_HPP)