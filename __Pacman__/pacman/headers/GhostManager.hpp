#ifndef PACMAN_GHOST_MANAGER_HPP
#define PACMAN_GHOST_MANAGER_HPP
#include "Ghost.hpp"
#include "Blinky.hpp"
#include "Pinky.hpp"
#include "Inky.hpp"
#include "Clyde.hpp"
#include <array>

namespace pacman {

class GhostManager {
public:
    struct ModeInterval {
        int duration;
        typename Ghost::Mode type;
    };

    struct Phase {
        MinMax<int> level_range;
        ModeInterval mode[9];
    };
    constexpr static Phase kPhases[] = {
        {MinMax<int>{1,1},      {{ 3_sec,Ghost::Mode::Chase },{ 7_sec,Ghost::Mode::Scatter },{20_sec,Ghost::Mode::Chase},{ 7_sec,Ghost::Mode::Scatter },{20_sec,Ghost::Mode::Chase},{5_sec,Ghost::Mode::Chase},{ 20_sec,Ghost::Mode::Chase },{ 5_sec,Ghost::Mode::Scatter},{ 99999_min,Ghost::Mode::Chase }}},
        {MinMax<int>{2,4},      {{ 3_sec,Ghost::Mode::Chase },{ 7_sec,Ghost::Mode::Scatter },{20_sec,Ghost::Mode::Chase},{ 7_sec,Ghost::Mode::Scatter },{20_sec,Ghost::Mode::Chase},{5_sec,Ghost::Mode::Chase},{ 17_min,Ghost::Mode::Chase },{ 5_sec,Ghost::Mode::Scatter},{ 99999_min,Ghost::Mode::Chase }}},
        {MinMax<int>{5,999999}, {{ 3_sec,Ghost::Mode::Chase },{ 5_sec,Ghost::Mode::Scatter },{20_sec,Ghost::Mode::Chase},{ 5_sec,Ghost::Mode::Scatter },{20_sec,Ghost::Mode::Chase},{5_sec,Ghost::Mode::Chase},{ 17_min,Ghost::Mode::Chase },{ 5_sec,Ghost::Mode::Scatter},{ 99999_min,Ghost::Mode::Chase }}}
    };

    void nextPhase();
    void tickTimers();
    void changePhase(int level);

    std::array<Ghost*,4> get() noexcept
    {
        return { &mBlinky, &mPinky, &mInky, &mClyde };
    }
    std::array<const Ghost*,4> get() const noexcept
    {
        return { &mBlinky, &mPinky, &mInky, &mClyde };
    }

    const Phase& getPhase(int level)
    {
        for (auto& phase : kPhases) {
            if (in_minmax_range(level, phase.level_range)) {
                return phase;
            }
        }
        assert(!"couldn't find phase with required level.");
        return kPhases[0];
    }
    void draw(sf::RenderTarget& target);
    void reset(const typename Map::Data& data);
    void update(const Map& map, sf::Vector2i pacman_pos, Direction pacman_direction);
    void setTargets(const Map& map, sf::Vector2i pacman_pos, Direction pacman_direction);
    void loadTextures(const TextureManager& manager);
    void init(Map& map);

private:
    Map* mMap;
    Blinky mBlinky;
    Pinky mPinky;
    Inky mInky;
    Clyde mClyde;
    int mTimeUntilNextPhase{ 0 };
    int mCurrentPhaseIndex{ 0 };
    bool mPhaseChanged{ true };
    typename Ghost::Mode mCurrentMode;
};

}
#endif // !PACMAN_GHOST_MANAGER_HPP
