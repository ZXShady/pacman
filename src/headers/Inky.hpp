#ifndef PACMAN_INKY_HPP
#define PACMAN_INKY_HPP

#include "Ghost.hpp"
#include "Globals.hpp"
#include "Map.hpp"

namespace pacman {

class Inky /*cyan*/ : public Ghost {
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
    void setTarget(sf::Vector2i pacman_pos, Direction pacman_direction, sf::Vector2i blinky_pos)
    {
        if (mMode == Mode::Chase) {
            mTargetPos = pacman_pos;
            //mTargetPos = nextPosition(pacman_pos, pacman_direction, kCellSize * kChaseDistance);
            //mTargetPos += mTargetPos - blinky_pos;

        }
    }

    //void drawBody(sf::RenderTarget& target) override
    //{
    //    if (mIsScared) {
    //        mSprite->setColor(sf::Color{ 40,40,255 });
    //    }
    //    else {
    //        mSprite->setColor(sf::Color{ 0,255,255 }); // cyan
    //    }
    //    mSprite->setTextureRect(makeIntRect(0, 0));
    //    target.draw(*mSprite);
    //}
private:
    constexpr static int kScatterMinDistance = 4;
    constexpr static int kChaseDistance = 2;
};

}

#endif // !PACMAN_INKY_HPP
