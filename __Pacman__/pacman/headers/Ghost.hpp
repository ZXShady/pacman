#ifndef PACMAN_GHOST_HPP_
#define PACMAN_GHOST_HPP_

#include <memory>
#include <algorithm>
#include <iostream>

#include <SFML/Graphics.hpp>

#include "Map.hpp"
#include "ResourceManager.hpp"

namespace pacman {

class Ghost {
public:
    enum class Color {
        None,
        Pink,
        Red,
        Cyan,
        Orange
    };
    enum class Mode {
        None,
        Chase,
        Scatter,
        Frightened,
        Eaten
    };

    Mode getMode() const noexcept { return mMode;}



    void reachedHome();
    void reachedSpawnPoint();

    void scatter();
    void frighten();
    void chase();
    void setMode(Mode mode);

    void loadTextures(const TextureManager& manager)
    {
        mSprite->setTexture(manager.get(TextureID::Ghost));
    }

    void die()
    {
        if (mMode == Mode::Eaten)
            return;

        mCanUseDoor = true;
        mTargetPos = mSpawnPointPos;
        mMode = Mode::Eaten;
        mFrightenedTimer = 0;
        mSpeed = kSpeedWhenEaten;
    }

    void tickTimers()
    {
        ++mAnimationTimer;
        if (mAnimationTimer >= kMaxAnimationTimer)
            mAnimationTimer = 0;

        if (mMode == Mode::Frightened) {
            --mFrightenedTimer;
            if (mFrightenedTimer < 0) {
                setMode(Mode::Chase);
            }
        }
    }
    //Direction calcOptimalDirection(const Map& map);
    void setSpeed(Mode mode);

    void update(const Map& map)
    {
        setSpeed(mMode);
        if (!map.intersectsIf(pos, [](const Cell& c) { return c.type == CellType::Glass; }).empty()) {
            mCanUseDoor = true;
        }

        if (-kCellSize >= pos.x)
            pos.x = kCellSize * kMapWidth - mSpeed;
        else if (kCellSize * kMapWidth - mSpeed <= pos.x)
            pos.x = mSpeed - kCellSize;

        if (-kCellSize >= pos.y)
            pos.y = kCellSize * kMapHeight - mSpeed;
        else if (kCellSize * kMapHeight - mSpeed <= pos.y)
            pos.y = mSpeed - kCellSize;

        const auto copySpeed = mSpeed;
        mSpeed = 1;

        for (int i=1; i <= copySpeed; i++) {
            if (mCanUseDoor) {
                if (pos == mTargetPos) {

                    if (mHomeExitPos == mTargetPos) {
                        reachedHome();
                    }
                    else if (mSpawnPointPos == mTargetPos) {
                        reachedSpawnPoint();
                    }
                }
            }
            
            
            
            
            

            auto wall = [&, this](Direction dir) -> bool
                {
                    return !map.intersectsIf(nextPosition(pos, dir, mSpeed), [&, this](Cell& c)
                             {
                                 return this->mCanUseDoor
                                     ? c.hasCollision()
                                     : (c.type == CellType::Glass || c.hasCollision());
                             }).empty();
                };

            int ways = 0;
            if (mMode != Mode::Frightened) {
                Direction optimal_direction = Direction::None;

                for (auto dir : Directions) {
                    if (dir == flipDirection(mDirection))
                        continue;

                    if (!wall(dir)) {
                        if (optimal_direction == Direction::None) {
                            optimal_direction = dir;
                        }

                        ways++;

                        if (pythagoras(nextPosition(pos, dir, mSpeed) - mTargetPos)
                          < pythagoras(nextPosition(pos, optimal_direction, mSpeed) - mTargetPos)) {
                            optimal_direction = dir;
                        }
                    }
                }

                if (ways > 1)
                    mDirection = optimal_direction;
                else if (optimal_direction == Direction::None)
                    mDirection = flipDirection(mDirection);
                else
                    mDirection = optimal_direction;
            }
            else {
                auto random_direction = static_cast<Direction>(rand() % 4 + 1);

                for (auto dir : Directions) {
                    if (dir == flipDirection(mDirection))
                        continue;

                    if (!wall(dir))
                        ways++;
                }

                if (ways > 0) {
                    while (wall(random_direction) || random_direction == flipDirection(mDirection))
                        random_direction = static_cast<Direction>(rand() % 4 + 1);
                    mDirection = random_direction;
                }
                else {
                    mDirection = flipDirection(mDirection);
                }
            }
            if (map.canMove(nextPosition(pos, mDirection, mSpeed))) {
                pos = nextPosition(pos, mDirection, mSpeed);
            }
        }
        mSpeed = copySpeed;
        tickTimers();
    }

    void drawBody(sf::RenderTarget& target)
    {
        if (getMode() == Mode::Eaten)
            return;

        mSprite->setPosition(static_cast<float>(pos.x),static_cast<float>(pos.y));
        mSprite->setTextureRect(makeIntRect(mAnimationTimer / (kMaxAnimationTimer / kAnimationFrameCount), 0));


        if (mMode == Mode::Frightened) {
            mSprite->setColor(sf::Color{ 0,0,255 });
            if (mFrightenedTimer < kFrightenedDuration / 5) {
                if (mFrightenedTimer % 0.3_sec < 0.15_sec) {
                    mSprite->setColor(sf::Color{ 255,255,255 });
                    //body.setColor(sf::Color(255, 255, 255));
                    //face.setColor(sf::Color(255, 0, 0));
                }
            }
        }
        else {
            switch (id) {
                case EntityID::None:
                    mSprite->setColor(sf::Color{ 128,128,128 }); // grey
                    break;
                case EntityID::Pinky:
                    mSprite->setColor(sf::Color{ 255,192,203 }); // pink
                    break;
                case EntityID::Inky:
                    mSprite->setColor(sf::Color{ 255,165,0 }); // orange
                    break;
                case EntityID::Clyde:
                    mSprite->setColor(sf::Color{ 0,255,255 }); // cyan
                    break;
                case EntityID::Blinky:
                    mSprite->setColor(sf::Color(255, 0, 0)); // red
                    break;
            }
        }
        target.draw(*mSprite);
    }


    void drawEyes(sf::RenderTarget& target)
    {
        if (mMode == Mode::Frightened) {
            mSprite->setTextureRect(makeIntRect(4, 1));
        }
        else {
            const bool is_eaten = mMode == Mode::Eaten;
            switch (mDirection) {
                case Direction::Right:
                    mSprite->setTextureRect(makeIntRect(0, 1+is_eaten));
                    break;
                case Direction::Left:
                    mSprite->setTextureRect(makeIntRect(2, 1+is_eaten));
                    break;
                case Direction::Up:
                    mSprite->setTextureRect(makeIntRect(1, 1+is_eaten));
                    break;
                case Direction::Down:
                    mSprite->setTextureRect(makeIntRect(3, 1+is_eaten));
                    break;
            }
        }

        mSprite->setColor(sf::Color::White);
        mSprite->setPosition(static_cast<float>(pos.x),static_cast<float>(pos.y));
        target.draw(*mSprite);
    }
    void draw(sf::RenderTarget& target)
    {
        drawBody(target);
        drawEyes(target);
    }

    void reset(const typename Map::Data& data)
    {
        mScatterPos = data.entities.at(id).scatter_pos;
        mHomeExitPos = data.exit_pos;
        pos = data.entities.at(id).starting_pos;
        mSpawnPointPos = pos;
        setMode(Mode::None);
        useDoor();
        mDirection = Direction::Up;
    }
    void useDoor() noexcept
    {
        mCanUseDoor = mSpawnPointPos != mHomeExitPos;
    }
public:
    EntityID id{ EntityID::None };
    std::unique_ptr<sf::Sprite> mSprite{ new sf::Sprite };
    sf::Vector2i pos;
    sf::Vector2i mTargetPos;
    int mSpeed{ 1 };
    bool mCanUseDoor{ false };
    int mFrightenedTimer{ 0 };
    int mAnimationTimer{ 0 };
    Mode mMode;

    sf::Vector2i mHomeExitPos;
    sf::Vector2i mSpawnPointPos;
    sf::Vector2i mScatterPos;
    Direction mDirection;
    
    constexpr static int kAnimationFrameCount  = 6;
    constexpr static int kMaxAnimationTimer    = 1_sec;

    constexpr static int kSpeedWhenChasing     = 1;
    constexpr static int kSpeedWhenRandom      = 1;
    constexpr static int kSpeedWhenScatter     = 1;
    constexpr static int kSpeedWhenEaten       = 3;
    constexpr static int kSpeedWhenFrightened  = 1;

};
}
#endif // !PACMAN_GHOST_HPP