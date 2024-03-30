#include <string>
#include <array>
#include "headers/Globals.hpp"
#include "headers/Game.hpp"
#include "headers/Pacman.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fstream>

using namespace pacman;

Game::Game()
	: mWindow(new sf::RenderWindow(sf::VideoMode{ 520,420 }, "===== PacMan ====", sf::Style::Default))
	, mTextureManager({
		{TextureID::Map, "res/images/map_16.png"},
		{TextureID::Pacman, "res/images/pacman_16.png"},
		{TextureID::Ghost, "res/images/ghost_16.png"},
		{TextureID::PacmanDeath, "res/images/pacman_death_16.png"}
	})
	, mSoundBufferManager({
		{SoundBufferID::Wa,"res/sounds/wa.wav"},
		{SoundBufferID::Ka,"res/sounds/ka.wav"},
		{SoundBufferID::EatGhost,"res/sounds/eat_ghost.wav"},
		{SoundBufferID::Death1,"res/sounds/death_1.wav"},
		{SoundBufferID::Death2,"res/sounds/death_2.wav"},
		{SoundBufferID::Energizer,"res/sounds/energizer.wav"},
		{SoundBufferID::GameStart,"res/sounds/game_start.wav"},
		{SoundBufferID::Intermission,"res/sounds/intermission.wav"},
		{SoundBufferID::RareLoss,"res/sounds/rare_loss.wav"}
	})
{
	loadFileNames();

	std::srand(static_cast<unsigned int>(std::time(0))); // a bad number generator but who cares!

	mPacman.loadTextures(mTextureManager);
	mPacman.loadSoundBuffers(mSoundBufferManager);
	mGhostManager.loadTextures(mTextureManager);
	mMap.loadTextures(mTextureManager);

	

	auto& data = mMap.loadSketch(mMapSketches[rand() % mMapSketches.size()]);
	
	mGhostManager.init(mMap);
	mPacman.pos = data.pacman_pos;
	mGhostManager.reset(mMap.data());
	mFont.loadFromFile("res/fonts/emulogic.ttf");
	mText.setFont(mFont);
}

void pacman::Game::loadFileNames()
{
	std::ifstream file;
	file.open("res/data/maps/text/__LIST__.txt");
	if (!file.is_open()) {
		std::puts("can't open res/data/maps/text/__LIST__.txt file or file does not exist");
		abort();
	}
	
	char ch;
	std::string str;
	while (file.get(ch)) {
		if (ch == '\n') {
			mTextMapFileNames.push_back(str);
			str.clear();
		}
		else {
			str.push_back(ch);
		}
	}

	if (file.eof()) {
		if(!str.empty())
			mTextMapFileNames.push_back(str);
	}

	str.clear();
	file.close();
	std::vector<std::string> sketch;
	for (const std::string& name : mTextMapFileNames) {
		file.open("res/data/maps/text/" + name + ".txt");
		if (!file.is_open()) {
			std::printf("can't open file or file not exist %s from __LIST__.txt file be sure to remove removed files\n",name.c_str());
			abort();
		}

		while (file >> std::noskipws >> ch) {
			if (ch == '\n') {
				sketch.push_back(str);
				str.clear();
			}
			else {
				str.push_back(ch);
			}
		}
		mMapSketches.push_back(sketch);
		sketch.clear();
		file.close();
	}
}

void Game::run() {
	sf::Time TimePerFrame = sf::seconds(1.0f / static_cast<float>(kFramesPerSecond));
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	while (mWindow->isOpen()) {
		processEvents();
		timeSinceLastUpdate += clock.restart();
		while (timeSinceLastUpdate > TimePerFrame) {
			timeSinceLastUpdate -= TimePerFrame;
			if (mGameState != GameState::Paused) {
				update(TimePerFrame);
			}
		}
		mFrame++;
		render();
	}
}

void Game::processEvents() {
	sf::Event event;
	while (mWindow->pollEvent(event)) {
		//mPacman.handleEvent(event,mMap);
		if (event.type == sf::Event::Closed)
			mWindow->close();
		if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::Key::P)
				mIsPaused = !mIsPaused;
			if (event.key.code == sf::Keyboard::Key::Enter)
				if(mPacman.isDead()) 
					lose();
		}
	}
}

void Game::update(sf::Time delta_time) {
	mTicksPassed++;
	if(mGameState==GameState::Lose || mGameState==GameState::Win)
		mWinOrLoseTicks++;

	if (mWinOrLoseTicks >= kWinOrLoseDuration) {
		if (mGameState == GameState::Lose)
			lose();
		else
			win();
		mWinOrLoseTicks = 0;
	}
	mMap.update();
	mPacman.update(mGhostManager, mMap);
	
	if (!mPacman.isDead()) {
		mGhostManager.update(mMap, mPacman.pos, mPacman.getDirection());
	}
	else {
		mGameState = GameState::Lose;
	}

	const auto count = mMap.countCells(CellType::Pellet) + mMap.countCells(CellType::Energizer);
	if (count == 0) {
		mGameState = GameState::Win;
	}
}

void Game::render() {
	mWindow->clear();
	if (mGameState != GameState::Win && mGameState != GameState::Lose) {
		mMap.draw(*mWindow);
		mGhostManager.draw(*mWindow);
	}
	mPacman.draw(*mWindow);

	if (mGameState == GameState::Win) {
		mText.setPosition(kCellSize*kMapWidth / 2, kCellSize*kMapHeight / 2);
		mText.setString("You Win! :D\nIt will get harder!");
		mText.setCharacterSize(16);
		mWindow->draw(mText);
	}

	if (mGameState == GameState::Lose) {
		mText.setPosition(kCellSize*kMapWidth / 2, kCellSize*kMapHeight / 2);
		mText.setString("You Lose!:(\n");
		mText.setCharacterSize(16);
		mWindow->draw(mText);
	}
	mWindow->display();
}

void pacman::Game::lose()
{
	mGameState = GameState::Playing;
	
	mLoseSound.setBuffer(mSoundBufferManager.get(SoundBufferID::RareLoss));
	if (rand() % 5 == 0) {
		mLoseSound.setVolume(70.0f);
		mLoseSound.play();
	}

	const Map::Data& data = mMap.loadSketch(mMapSketches[rand() % mMapSketches.size()]);
	mPacman.reset(data);
	mGhostManager.reset(data);
}

void pacman::Game::win()
{
	mGameState = GameState::Playing;

	mWinSound.setBuffer(mSoundBufferManager.get(SoundBufferID::Intermission));
	mWinSound.play();

	const Map::Data& data = mMap.loadSketch(mMapSketches[rand() % mMapSketches.size()]);
	mMap.data().level++;
	mGhostManager.init(mMap);
	mPacman.reset(data);
	mGhostManager.reset(mMap.data());
}
