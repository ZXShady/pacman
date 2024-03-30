#ifndef PACMAN_GHOST_MANAGER_HPP
#define PACMAN_GHOST_MANAGER_HPP

#include "Ghost.hpp"
#include "Blinky.hpp"
#include "Pinky.hpp"
#include "Inky.hpp"
#include "Clyde.hpp"
#include "ResourceManager.hpp"

#include <SFML/Audio/Sound.hpp>

#include <array>
namespace pacman {

class GhostManager {
public:

    struct ModeDuration {
        int duration;
        typename Ghost::Mode type;
    };

    struct Phase {
        MinMax<int> level_range;
        ModeDuration modes[9];
    };
    constexpr static Phase kPhases[] = {
        {MinMax<int>{1,1},      {{ 3_sec,Ghost::Mode::Chase },{ 7_sec,Ghost::Mode::Scatter },{20_sec,Ghost::Mode::Chase},{ 7_sec,Ghost::Mode::Scatter },{20_sec,Ghost::Mode::Chase},{5_sec,Ghost::Mode::Chase},{ 20_sec,Ghost::Mode::Chase },{ 5_sec,Ghost::Mode::Scatter},{ 99999_min,Ghost::Mode::Chase }}},
        {MinMax<int>{2,4},      {{ 3_sec,Ghost::Mode::Chase },{ 7_sec,Ghost::Mode::Scatter },{20_sec,Ghost::Mode::Chase},{ 7_sec,Ghost::Mode::Scatter },{20_sec,Ghost::Mode::Chase},{5_sec,Ghost::Mode::Chase},{ 17_min,Ghost::Mode::Chase },{ 5_sec,Ghost::Mode::Scatter},{ 99999_min,Ghost::Mode::Chase }}},
        {MinMax<int>{5,999999}, {{ 3_sec,Ghost::Mode::Chase },{ 5_sec,Ghost::Mode::Scatter },{20_sec,Ghost::Mode::Chase},{ 5_sec,Ghost::Mode::Scatter },{20_sec,Ghost::Mode::Chase},{5_sec,Ghost::Mode::Chase},{ 17_min,Ghost::Mode::Chase },{ 5_sec,Ghost::Mode::Scatter},{ 99999_min,Ghost::Mode::Chase }}}
    };

    void startNextPhase();
    void tickTimers();
    void changePhase(int level);
    void loadSounds(const SoundBufferManager& manager)
    {
        mGhostSirenSound.setBuffer(manager.get(SoundBufferID::GhostSiren));
    }
    std::array<Ghost*,4> get() noexcept
    {
        return { &mBlinky, &mPinky, &mInky, &mClyde };
    }
    std::array<const Ghost*,4> get() const noexcept
    {
        return { &mBlinky, &mPinky, &mInky, &mClyde };
    }

    void draw(sf::RenderTarget& target);
    void reset(const typename Map::Data& data);
    void update(const Map& map, sf::Vector2i pacman_pos, Direction pacman_direction);
    void setTargets(const Map& map, sf::Vector2i pacman_pos, Direction pacman_direction);
    void loadTextures(const TextureManager& manager);
    void init(const Map& map);

private:
    const Map* mMap;
    Blinky mBlinky;
    Pinky mPinky;
    Inky mInky;
    Clyde mClyde;
    int mTimeUntilNextPhase{ 0 };
    int mCurrentModeIndex{ 0 };
    int mTimeUntilGhostSiren{ 3.5_sec };
    typename Ghost::Mode mCurrentMode;
    typename Ghost::Mode mNextMode;
    sf::Sound mGhostSirenSound;
};

}
#endif // !PACMAN_GHOST_MANAGER_HPP
