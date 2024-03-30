#ifndef PACMAN_UTILITIES_HPP
#define PACMAN_UTILITIES_HPP

#include <cstdint>

namespace pacman {

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

}

#endif // !defined(PACMAN_UTILITIES_HPP)