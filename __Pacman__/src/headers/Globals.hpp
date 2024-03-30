#ifndef PACMAN_GLOBALS_HPP
#define PACMAN_GLOBALS_HPP

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <type_traits>
#include <cmath>

namespace pacman {

enum class EntityID {
    None,
    Pinky,
    Inky,
    Blinky,
    Clyde,
    Pacman
};

template<typename Numeric>
struct MinMax {
    Numeric min;
    Numeric max;
};

template<typename Numeric,typename MinMaxT>
constexpr inline bool in_minmax_range(Numeric value, MinMaxT range)
{
    return range.min >= value && value <= range.max;
}
constexpr int kFramesPerSecond = 60;


template<typename Numeric>
constexpr int secondsToTicks(Numeric count) noexcept
{
    return static_cast<int>(count * kFramesPerSecond);
}

constexpr int operator""_sec(long double count) noexcept
{
    return secondsToTicks(count);
}
constexpr int operator""_sec(unsigned long long count) noexcept
{
    return secondsToTicks(count);
}

constexpr int operator""_min(long double count) noexcept
{
    return 60 * secondsToTicks(count);
}
constexpr int operator""_min(unsigned long long count) noexcept
{
    return 60 * secondsToTicks(count);
}


constexpr int kCellSize = 16;
constexpr int kTextureSize = 16;
constexpr int kMapHeight = 21;
constexpr int kMapWidth  = 21;
constexpr int kTicksPerSecond = kFramesPerSecond / 3;
constexpr int kEnergizerDuration = 7_sec;
constexpr int kFrightenedDuration = 7_sec;

template<typename Float1,typename Float2>
auto pythagoras(Float1 x, Float2 y) -> decltype(::std::sqrt(x * x + y * y))
{
    return ::std::sqrt(x * x + y * y);
}

template<typename Vector>
auto pythagoras(Vector vector) -> decltype(pythagoras(vector.x,vector.y))
{
    return pythagoras(vector.x, vector.y);
}



enum struct Direction {
    None,
    Left,
    Up,
    Right,
    Down
};

constexpr Direction Directions[] = {
    Direction::Left,
    Direction::Up,
    Direction::Right,
    Direction::Down
};

inline Direction flipDirection(Direction direction)
{
    switch (direction) {
        case Direction::Left:
            return Direction::Right;
        case Direction::Right:
            return Direction::Left;
        case Direction::Up:
            return Direction::Down;
        case Direction::Down:
            return Direction::Up;
   }
   return Direction::None;
   //return direction == Direction::Left  ? Direction::Right
   //    :  direction == Direction::Right ? Direction::Left
   //    :  direction == Direction::Up    ? Direction::Down
   //    :  direction == Direction::Down  ? Direction::Up
   //    :  Direction::None;


}



enum struct CellType {
    Empty,
    Wall,
    Pellet,
    Glass,
    Energizer
};

inline sf::IntRect makeIntRect(int x, int y,int z = 1,int w = 1) noexcept
{
    return sf::IntRect(x * kTextureSize, y * kTextureSize,z * kTextureSize, w * kTextureSize);
}

inline bool random_chance(int one_in) noexcept 
{
    return rand() % one_in == 0;
}

inline sf::Vector2i nextPosition(sf::Vector2i pos,Direction direction,int speed,double percentage = 1.0,bool forward = true) noexcept 
{
    speed = static_cast<int>(percentage * (forward ? speed : -speed));

    switch (direction) {
        case Direction::Right:
            pos.x += speed;
            break;
        case Direction::Left:
            pos.x -= speed;
            break;
        case Direction::Up:
            pos.y -= speed;
            break;
        case Direction::Down:
            pos.y += speed;
            break;
    }
    return pos;
}
struct Cell {
    CellType type;
    sf::Vector2i pos;
    sf::Color color{ sf::Color::White };
    sf::IntRect getIntRect() const noexcept
    {

        switch (type) {
            case CellType::Wall: return makeIntRect(0, 0);
            case CellType::Pellet: return makeIntRect(0, 1);
            case CellType::Glass: return makeIntRect(2, 1);
            case CellType::Energizer: return makeIntRect(1, 1);
        }
        return { 0,0,0,0 };
    }
    bool hasCollision() const noexcept
    {
        return type == CellType::Wall;
    }

    bool isEdible() const noexcept
    {
        return type == CellType::Pellet
            || type == CellType::Energizer;
    }
};



}

#endif // !PACMAN_GLOBALS_HPP