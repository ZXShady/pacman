#ifndef PACMAN_BLINKY_HPP
#define PACMAN_BLINKY_HPP

#include "Ghost.hpp"
#include "Globals.hpp"
#include "Map.hpp"

namespace pacman {

class Blinky : public Ghost {
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
    void setTarget(sf::Vector2i pacman_pos, const Map& map)
    {
        const auto current_pellet_count = map.countCells(CellType::Pellet) + map.countCells(CellType::Energizer);
        const auto total_pellet_count = map.data().cell_count.at(CellType::Pellet) + map.data().cell_count.at(CellType::Energizer);
        const auto level = map.data().level;
        struct FakeScatter {
            MinMax<int> level;
            int percent;
        };
        constexpr static FakeScatter fake[] = {
            {{1,1},      (20*100)/244},
            {{2,5},      (30*100)/244},
            {{6,8},      (40*100)/244},
            {{9,14},     (50*100)/244},
            {{15,999999},(60*100)/244}
        };
        if (mMode == Mode::Chase) {
            mTargetPos = pacman_pos;
            return;
        }
        if (mMode == Mode::Scatter) {
            const auto percent = ((current_pellet_count*100) / total_pellet_count);
            for (auto i : fake) {
                if (in_minmax_range(level, i.level) && percent <= (i.percent / 100)) {
                    setMode(Mode::Chase);
                    return;
                }
            }
            mTargetPos = mScatterPos;
            return;
        }
        if (mMode == Mode::Eaten) {
            mTargetPos = mSpawnPointPos;
            return;
        }
    }

    //void drawBody(sf::RenderTarget& target) override
    //{
    //    if (mIsScared) {
    //        mSprite->setColor(sf::Color{ 40,40,255 });
    //    }
    //    else {
    //        mSprite->setColor(sf::Color(255, 0, 0)); // red
    //    }
    //    mSprite->setTextureRect(makeIntRect(0, 0));
    //    target.draw(*mSprite);
    //}
};

}

#endif