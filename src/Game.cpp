#include <string>
#include <array>
#include "headers/Globals.hpp"
#include "headers/Game.hpp"
#include "headers/Pacman.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fstream>


pacman::Game::Game()
	: mWindow(new sf::RenderWindow(sf::VideoMode{
	static_cast<int>(kCellSize * kMapWidth*2), static_cast<int>((kCellSize * kMapHeight) * 2)
	}, "PacMan", sf::Style::Default))
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
		{SoundBufferID::RareLoss,"res/sounds/rare_loss.wav"},
		{SoundBufferID::GhostSiren,"res/sounds/ghost_siren.wav"}
	})
{
	mWindow->setView(sf::View(sf::FloatRect(0, 0, kCellSize * kMapWidth,
		kCellSize * kMapHeight*1.25)));
	loadFiles();

	mLivesSprite->setTexture(mTextureManager.get(TextureID::Pacman));
	std::srand(static_cast<unsigned int>(std::time(0))); // a bad number generator but who cares!

	mPacman.loadTextures(mTextureManager);
	mPacman.loadSoundBuffers(mSoundBufferManager);
	
	mGhostManager.loadTextures(mTextureManager);
	mGhostManager.loadSounds(mSoundBufferManager);
	
	mMap.loadTextures(mTextureManager);
	mMap.loadSketch(mMapSketches[std::rand() % mMapSketches.size()]);
	
	mGhostManager.init(mMap);
	
	mPacman.pos = mMap.data.pacman_pos;
	mGhostManager.reset(mMap.data);
	mFont.loadFromFile("res/fonts/emulogic.ttf");
	mText.setFont(mFont);
}

void pacman::Game::loadFiles()
{
	std::string str;
	std::fstream file;
	file.open("res/data/high_score.txt");
	if (!file.is_open()) {
		file.open("res/data/high_score.txt", std::ofstream::out);
		mPacman.high_score = 0;
		if (!file.is_open()) {
			std::puts("can't open res/data/high_score.txt file or file does not exist");
			abort();
		}
	}
	else {
		file >> mPacman.high_score;
	}
	file.close();

	file.open("res/data/high_score.txt");
	if (!file.is_open()) {
		std::puts("can't open res/data/high_score.txt file or file does not exist");
		abort();
	}
	file.close();

	file.open("res/data/maps/text/__LIST__.txt");
	if (!file.is_open()) {
		std::puts("can't open res/data/maps/text/__LIST__.txt file or file does not exist");
		abort();
	}
	
	char ch;
	str.clear();
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

void pacman::Game::run() {
	sf::Clock clock;
	sf::Time time_since_last_update = sf::Time::Zero;
	sf::Clock fps_clock;
	while (mWindow->isOpen()) {
		if (mGameSpeed > 10.0f) {
			mGameSpeed = 10.0f;
		}
		else if (mGameSpeed < 0.01f) {
			mGameSpeed = 0.01f;
		}

		const sf::Time time_per_frame = sf::seconds((1.0f / static_cast<float>(kFramesPerSecond)) / mGameSpeed);
		processEvents();
		time_since_last_update += clock.restart();
		while (time_since_last_update > time_per_frame) {
			time_since_last_update -= time_per_frame;
			if (mGameState != GameState::Paused) {
				update(time_per_frame);
			}
		}
		mFps = static_cast<int>(1.0f / fps_clock.restart().asSeconds());
		render();
	}
}

void pacman::Game::processEvents()
{
	sf::Event event;
	while (mWindow->pollEvent(event)) {
		//mPacman.handleEvent(event,mMap);
		if (event.type == sf::Event::Closed)
			exit();
		if (event.type == sf::Event::KeyPressed) {
			switch (event.key.code) {
				case sf::Keyboard::Key::P:
					if (mGameState == GameState::Playing)
						mGameState = GameState::Paused;
					else if (mGameState == GameState::Paused)
						mGameState = GameState::Playing;
					break;
				case sf::Keyboard::Key::Enter:
					if (mPacman.isDead())
						lose();
					break;
				case sf::Keyboard::Key::R:
					reset();
					break;
				case sf::Keyboard::Key::Up:
					mGameSpeed += kGameSpeedIncrease;
					break;
				case sf::Keyboard::Key::Down:
					mGameSpeed += kGameSpeedDecrease;
					break;
				case sf::Keyboard::Key::C:
					mDebugInfoOn = !mDebugInfoOn;
					break;
			}
		}
	}
}

void pacman::Game::update(sf::Time delta_time) 
{
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
	if (mGameState == GameState::Playing) {
		mMap.update();
		mPacman.update(mGhostManager, mMap);
		mGhostManager.update(mMap, mPacman.pos, mPacman.getDirection());
	}
	if (mPacman.isDead()) {
		mGameState = GameState::Lose;
	}

	const auto count = mMap.countCells(CellType::Pellet) + mMap.countCells(CellType::Energizer);
	if (count == 0) {
		mGameState = GameState::Win;
	}
}

void pacman::Game::render()
{
	mWindow->clear();
	if (mGameState != GameState::Win && mGameState != GameState::Lose) {
		mMap.draw(*mWindow);
		mGhostManager.draw(*mWindow);
		mPacman.draw(*mWindow);
		std::vector<std::string> strs = {
			std::string("Level: ")
			+ std::to_string(mMap.data.level),
			"Score: "
			+ std::to_string(mPacman.score)
			+ " | High Score: "
			+ std::to_string(mPacman.high_score)
		};


		if (mDebugInfoOn) {
			if (mFps >= 999) {
				strs.push_back("FPS: +999 | Game Speed: " + std::to_string(mGameSpeed));
			}
			else if (mFps >= 99) {
				strs.push_back("FPS:  " + std::to_string(mFps) + " | Game Speed: " + std::to_string(mGameSpeed));
			}
			else if(mFps >= 9) {
				strs.push_back("FPS:  0" + std::to_string(mFps) + " | Game Speed: " + std::to_string(mGameSpeed));
			}
			else {
				strs.push_back("FPS:  00" + std::to_string(mFps) + " | Game Speed: " + std::to_string(mGameSpeed));
			}
		}
		int x = 0;
		int y = 0;
		mText.setCharacterSize(8);
		for (const auto& s : strs) {
			y += mText.getCharacterSize();
			mText.setString(s);
			mText.setPosition(
				0.0f,
				kCellSize * kMapHeight + y);
			mWindow->draw(mText);
		}
		mLivesSprite->setTextureRect(makeIntRect(1, 1));
		for (int cnt = 0; cnt < mLives; ++cnt) {
			auto pos = mText.getPosition();
			mLivesSprite->setPosition(pos.x, pos.y + 2 * mText.getCharacterSize());
			mWindow->draw(*mLivesSprite);
		}
	}
	if (mGameState == GameState::Win) {
		const std::string strs[] = {
			"You Win! :D",
			"It will get harder!"
		};
		int x = 0;
		int y = 0;
		mText.setCharacterSize(16);
		for (const auto& s : strs) {
			y += mText.getCharacterSize();
			mText.setString(s);
			mText.setPosition(
				kCellSize * kMapWidth / 2-(16*mText.getString().getSize()/2),
				kCellSize * kMapHeight / 2+y);
			mWindow->draw(mText);
		}
	}

	if (mGameState == GameState::Lose) {
		mText.setCharacterSize(16);
		mText.setString("You Lose! :(");
		mText.setPosition(
			kCellSize * kMapWidth / 2-(mText.getCharacterSize()*mText.getString().getSize()/2),
			kCellSize * kMapHeight / 2+mText.getCharacterSize());
	}


	mWindow->draw(mText);
	mWindow->display();
}

void pacman::Game::lose()
{
	save();
	mGameState = GameState::Playing;
	--mLives;
	if (mLives <= 0) {
		mMap.data.level = 1;
		reset();
		mLives = 3;
	}
	else {
		mPacman.reset(mMap.data);
		mGhostManager.reset(mMap.data);
	}
	
	mLoseSound.setBuffer(mSoundBufferManager.get(SoundBufferID::RareLoss));
	if (rand() % 5 == 0) {
		mLoseSound.setVolume(70.0f);
		mLoseSound.play();
	}
}

void pacman::Game::exit()
{
	save();
	mWindow->close();
}

void pacman::Game::save()
{
	if (mPacman.score > mPacman.high_score) {
		mPacman.high_score = mPacman.score;
	}
	std::ofstream file;
	file.open("res/data/high_score.txt", std::ofstream::out | std::ofstream::trunc);
	if (!file.is_open()) {
		std::puts("can't open res/data/high_score.txt file or file does not exist");
		abort();
	}
	file << mPacman.high_score;
	file.close();
}

void pacman::Game::reset()
{
	mMap.loadSketch(mMapSketches[rand() % mMapSketches.size()]);
	mPacman.reset(mMap.data);
	mGhostManager.reset(mMap.data);
}

void pacman::Game::win()
{
	if (mLives < 3)
		++mLives;
	mGameState = GameState::Playing;

	mWinSound.setBuffer(mSoundBufferManager.get(SoundBufferID::Intermission));
	mWinSound.play();

	++mMap.data.level;
	reset();
}
