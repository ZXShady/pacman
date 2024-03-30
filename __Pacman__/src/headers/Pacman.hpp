#ifndef PACMAN_PACMAN_HPP
#define PACMAN_PACMAN_HPP

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>

#include <vector>
#include <iostream>

#include "Globals.hpp"
#include "Map.hpp"
#include "GhostManager.hpp"


namespace pacman {

enum struct PacmanState {
	None,
	Lost
};

class Pacman {
public:
	void loadTextures(const TextureManager& manager);
	void loadSoundBuffers(const SoundBufferManager& manager);
	//void handleEvent(sf::Event event, const Map& map);


	void reset(const typename Map::Data& data);
	bool collidesWithEntity(sf::Vector2i position) const noexcept
	{
		return position.x > pos.x - kCellSize && position.x < kCellSize + pos.x
			&& position.y > pos.y - kCellSize && position.y < kCellSize + pos.y;
	}

	void die() noexcept
	{
		mIsDead = true;
		mDeathSound.setBuffer(mSoundBufferManager->get(SoundBufferID::Death1));
		//	mDeathSound.setBuffer(mSoundBufferManager->get(SoundBufferID::Death2));
		mDeathSound.play();
	
	}

	void tickTimers() noexcept 
	{
		++mAnimationTimer;
		if (mAnimationTimer >= getMaxAnimationTimer()) {
			mAnimationTimer = 0;
		}

		if (mEnergizerTimer > 0) {
			--mEnergizerTimer;
		}

		if(isDead() && mState != PacmanState::Lost) {
			mDeathAnimationTimer++;
			if (mDeathAnimationTimer > kDeathAnimationDuration) {
				mDeathAnimationTimer = 0;
				mState = PacmanState::Lost;
			}
		}

	}
	void update(GhostManager& ghosts, Map& map);

	bool canMove(Direction direction, const Map& map) const noexcept
	{
		return map.collides(nextPosition(pos, direction, mSpeed)).empty();
	}

	void draw(sf::RenderTarget& target)
	{
		int x = 0;
		int y = 0;
		auto& sprite = isDead() ? *mDeathAnimationSprite : *mAnimationSprite;
		switch (mDirection) {
			case Direction::Left:
				y = kTextureSize;
				sprite.setRotation(-90);
				break;
			case Direction::Right:
				x = kTextureSize;
				sprite.setRotation(90);
				break;
			case Direction::Up:
				sprite.setRotation(0);
				break;
			case Direction::Down:
				x = kTextureSize;
				y = kTextureSize;
				sprite.setRotation(180);
				break;
			case Direction::None:
				sprite.setRotation(0);
				break;
		}

		if (!isDead()) {
			if (mDirection == Direction::None)
				sprite.setTextureRect(makeIntRect(0, 0));
			else
				sprite.setTextureRect(makeIntRect(mAnimationTimer / (getMaxAnimationTimer() / kAnimationFrameCount), 1));
		}
		else if(mState != PacmanState::Lost){
			sprite.setTextureRect(makeIntRect(mDeathAnimationTimer / (kDeathAnimationDuration / kDeathAnimationFrameCount), 0));
			sprite.setPosition(static_cast<float>(pos.x), static_cast<float>(pos.y));
		}
			sprite.setPosition(static_cast<float>(pos.x + x), static_cast<float>(pos.y + y));
			target.draw(sprite);
	}

	Direction getDirection() const noexcept { return mDirection; }
	bool isEnergized() const noexcept { return mEnergizerTimer > 0; }
	bool isDead() const noexcept { return mIsDead; }
	int getMaxAnimationTimer() const noexcept
	{
		return 1_sec / mSpeed;
	}
public:
	sf::Vector2i pos;
	std::unique_ptr<sf::Sprite> mAnimationSprite{ new sf::Sprite };
	std::unique_ptr<sf::Sprite> mDeathAnimationSprite{ new sf::Sprite };
	int mSpeed{ 2 };
	static constexpr int kAnimationFrameCount = 5;
	static constexpr int kDeathAnimationFrameCount = 12;
	static constexpr int kDeathAnimationDuration   = 1_sec;

	bool mCanMove{ false };
	bool mIsDead{ false };
	Direction mDirection{ Direction::None };
	int mEatenPellets{ 0 };
	int mAnimationTimer{ 0 };
	int mEnergizerTimer{ 0 };
	int mDeathAnimationTimer{ 0 };
	sf::Sound mKaSound;
	sf::Sound mWaSound;
	sf::Sound mEatGhostSound;
	sf::Sound mDeathSound;
	sf::Sound mEnergizerSound;

	const TextureManager* mTextureManager;
	const SoundBufferManager* mSoundBufferManager;
	PacmanState mState{ PacmanState::None };
};

}
#endif // !PACMAN_PACMAN_HPP
