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
    enum class Mode {
        None,
        Chase,
        Scatter,
        Frightened,
        Eaten,
        ReachTarget
    };
    Mode getMode() const noexcept { return mMode;}



    void reachedHome();
    void reachedHomeExit();

    void scatter();
    void frighten();
    void chase();
    void setMode(Mode mode);

    void loadTextures(const TextureManager& manager)
    {
        mSprite->setTexture(manager.get(TextureID::Ghost));
    }

    //void frightenedStateEnded();
    void die();

    void tickTimers()
    {
        ++mAnimationTimer;
        if (mAnimationTimer >= kMaxAnimationTimer)
            mAnimationTimer = 0;

        if (mFrightenedTimer > 0) {
            --mFrightenedTimer;
            if (mFrightenedTimer == 0) {
                setMode(Mode::None);
            }
        }
    }
    void setSpeed(Mode mode);

    void update(const Map& map);

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
        mLevel = data.level;
        mFrightenedTimer = 0;
        mAnimationTimer = 0;
        mTimeUntilNextMode = 0;
        mScatterPos = data.entities.at(id).scatter_pos;
        mHomeExitPos = data.exit_pos;
        pos = data.entities.at(id).starting_pos;
        mSpawnPointPos = pos;
        mHomePos = data.glass[0].pos;
        mHomePos.y += kCellSize;
        mTargetPos = mHomeExitPos;
        setMode(Mode::ReachTarget);
        useDoor();
        mDirection = Direction::Up;
    
    }

    void useDoor() noexcept
    {
        mCanUseDoor = mSpawnPointPos != mHomeExitPos;
    }
public:
    Map* mMap;
    EntityID id{ EntityID::None };
    std::unique_ptr<sf::Sprite> mSprite{ new sf::Sprite };
    sf::Vector2i pos;
    sf::Vector2i mTargetPos;
    int mSpeed{ 1 };
    bool mCanUseDoor{ false };
    int mFrightenedTimer{ 0 };
    int mAnimationTimer{ 0 };
    Mode mMode{ Mode::ReachTarget };
    sf::Vector2i mHomeExitPos;
    sf::Vector2i mSpawnPointPos;
    sf::Vector2i mHomePos;
    sf::Vector2i mScatterPos;
    Direction mDirection;
    int mTimeUntilNextMode{ 0 };
    int mNextModeIndex;
    int mLevel{ 0 };
    constexpr static int kAnimationFrameCount  = 6;
    constexpr static int kMaxAnimationTimer    = 1_sec;

    constexpr static int kSpeedWhenChasing     = 1;
    constexpr static int kSpeedWhenRandom      = 1;
    constexpr static int kSpeedWhenScatter     = 1;
    constexpr static int kSpeedWhenEaten       = 3;
    constexpr static int kSpeedWhenFrightened  = 1;
    constexpr static int kSpeedWhenReachingTarget  = 1;
};
}
#endif // !PACMAN_GHOST_HPP