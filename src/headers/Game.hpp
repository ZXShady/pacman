#ifndef PACMAN_GAME_HPP
#define PACMAN_GAME_HPP
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

#include "ResourceManager.hpp"
#include "Globals.hpp"
#include "Pacman.hpp"
#include "Map.hpp"
#include <cmath>
#include "Blinky.hpp"
#include "Inky.hpp"
#include "Pinky.hpp"
#include "Clyde.hpp"
#include "GhostManager.hpp"

namespace pacman {
enum struct GameState {
    Playing,
    Paused,
    Lose,
    Win
};
class Game {
public:
    Game();
    void loadFiles();
    void run();
    void processEvents();
    void update(sf::Time delta_time);
    void render();
    void win();
    void lose();
    void exit();
    void save();
    void reset();
private:
    std::unique_ptr<sf::Sprite> mLivesSprite{ new sf::Sprite };
    int mLives{ 3 };
    int mFps{ 0 };
    int mTicksPassed{ 0 };
    int mWinOrLoseTicks{ 0 };
    static constexpr int kWinOrLoseDuration = 3_sec;
    GameState mGameState{GameState::Playing};
    Map mMap;
    std::unique_ptr<sf::RenderWindow> mWindow;
    TextureManager mTextureManager;
    SoundBufferManager mSoundBufferManager;
    Pacman mPacman;
    GhostManager mGhostManager;
    std::vector<std::string> mTextMapFileNames;
    std::vector<std::vector<std::string>> mMapSketches;
    sf::Font mFont;
    sf::Text mText;

    sf::Sound mLoseSound;
    sf::Sound mWinSound;
    float mGameSpeed{ 1.0f };
    bool mDebugInfoOn{ false };
    static constexpr float kGameSpeedIncrease = +0.1f;
    static constexpr float kGameSpeedDecrease = -0.1f;

};

}


#endif // !defined(PACMAN_GAME_HPP)
