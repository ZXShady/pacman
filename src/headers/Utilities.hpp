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

template<typename T, typename Value>
bool contains(const T& t,const Value& value) noexcept
{
    for (const auto& v : t)
        if (v == value)
            return true;
    return false;
}

template<typename T, typename Pred>
bool containsIf(const T& t,Pred pred) noexcept
{
    for (const auto& v : t)
        if (pred(v))
            return true;
    return false;
}

}

#endif // !defined(PACMAN_UTILITIES_HPP)