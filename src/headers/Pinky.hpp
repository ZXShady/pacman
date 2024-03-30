#ifndef PACMAN_pinky_HPP
#define PACMAN_pinky_HPP


#include "Ghost.hpp"
#include "Globals.hpp"
#include "Map.hpp"

namespace pacman {

class Pinky : public Ghost {
public:
    //                    case GhostColor::None:
    //                        mSprite->setColor(sf::Color{ 128,128,128 }); // grey
    //                        break;
    //                    case GhostColor::Pink:
    //                        mSprite->setColor(sf::Color{ 255,192,203 }); // pink
    //                        break;
    //                    case GhostColor::Orange:
    //                        mSprite->setColor(sf::Color{ 255,165,0 }); // orange
    //                        break;
    //                    case GhostColor::Cyan:
    //                        mSprite->setColor(sf::Color{ 0,255,255 }); // cyan
    //                        break;
    //                    case GhostColor::Red:

    void setTarget(sf::Vector2i pacman_pos, Direction pacman_direction)
    {
        if (mMode == Mode::Chase) {
            mTargetPos = nextPosition(pacman_pos, pacman_direction, kChaseDistance * kCellSize);
            if (pacman_direction == Direction::Up) {
                mTargetPos.x -= kChaseDistance * kCellSize;
            }
        
        }
    }

    //void drawBody(sf::RenderTarget& target) override
    //{
    //    if (mIsScared) {
    //        mSprite->setColor(sf::Color{ 40,40,255 });
    //    }
    //    else {
    //        mSprite->setColor(sf::Color(255,192,203)); // red
    //    }
    //    mSprite->setTextureRect(makeIntRect(0, 0));
    //    target.draw(*mSprite);
    //}
private:
    static constexpr int kChaseDistance = 4;
};

}


#endif // !PACMAN_pinky_HPP
