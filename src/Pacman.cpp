#include "headers/Pacman.hpp"


void pacman::Pacman::loadTextures(const TextureManager& manager)
{
	mTextureManager = &manager;
	mAnimationSprite->setTexture(manager.get(TextureID::Pacman));
	mDeathAnimationSprite->setTexture(manager.get(TextureID::PacmanDeath));
}

void pacman::Pacman::loadSoundBuffers(const SoundBufferManager& manager)
{
	mSoundBufferManager = &manager;
	mWaSound.setBuffer(manager.get(SoundBufferID::Wa));
	mKaSound.setBuffer(manager.get(SoundBufferID::Ka));
	mEatGhostSound.setBuffer(manager.get(SoundBufferID::EatGhost));
	mEnergizerSound.setBuffer(manager.get(SoundBufferID::Energizer));

}

void pacman::Pacman::reset(const typename Map::Data& data)
{
	pos = data.pacman_pos;
	mIsDead = false;
	mDirection = Direction::None;
	mEatenPellets = 0;
	mAnimationTimer = 0;
	mEnergizerTimer = 0;
	mDeathAnimationTimer = 0;
	mState = PacmanState::None;
}

void pacman::Pacman::tickTimers() noexcept
{
	++mAnimationTimer;
	if (mAnimationTimer >= getMaxAnimationTimer()) {
		mAnimationTimer = 0;
	}

	if (mEnergizerTimer > 0) {
		--mEnergizerTimer;
	}

	if(isDead()) {
		mDeathAnimationTimer++;
	}

}

void pacman::Pacman::update(GhostManager& ghosts, Map& map)
{
	for (auto spd = mSpeed; spd > 0; spd--) {
		if (!isDead()) {
			for (Ghost* ghost : ghosts.get()) {
				//printf("ghost %p           %d \n", (void*)ghost,ghost->color);
				if (collidesWithEntity(ghost->pos)) {
					if (isEnergized() && ghost->getMode() == Ghost::Mode::Frightened) {
						ghost->die();

						score += kEatingGhostScoreCount;
						mEatGhostSound.play();
					}
					else if (!isDead() && ghost->getMode() != Ghost::Mode::Eaten) {
						die();
						score += kDyingScoreCount;

					}
				}
			}
			
			
			if (-kCellSize >= pos.x)
				pos.x = kCellSize * kMapWidth - mSpeed;
			else if (kCellSize * kMapWidth - mSpeed <= pos.x)
				pos.x = mSpeed - kCellSize;
			
			if (-kCellSize >= pos.y)
				pos.y = kCellSize * kMapHeight - mSpeed;
			else if (kCellSize * kMapHeight - mSpeed <= pos.y)
				pos.y = mSpeed - kCellSize;

			Direction dir = Direction::None;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)
				&& map.collides(nextPosition(pos,Direction::Up,1)).empty())
				dir = Direction::Up;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)
				&& map.collides(nextPosition(pos,Direction::Down,1)).empty())
				dir = Direction::Down;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)
				&& map.collides(nextPosition(pos,Direction::Left,1)).empty())
				dir = Direction::Left;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)
				&& map.collides(nextPosition(pos,Direction::Right,1)).empty())
				dir = Direction::Right;

			if (dir != Direction::None)
				mDirection = dir;

			for (Cell* cell : map.intersects(nextPosition(pos, mDirection, 1))) {
				if (cell->isEdible()) {
					if (cell->type == CellType::Energizer) {
						score += kEatingEnergizerScoreCount;
						mEnergizerTimer = std::max(4_sec, kEnergizerDuration - (10 * map.data.level - 1));
						mEnergizerSound.play();
						for (Ghost* ghost : ghosts.get()) {
							if (ghost->getMode() != Ghost::Mode::Eaten) {
								ghost->frighten();
							}
						}
					}


					cell->type = CellType::Empty;
					++mEatenPellets;
					if (mEatenPellets % 2)
						mWaSound.play();
					else
						mKaSound.play();
					score += kEatingPelletScoreCount;
				}
			}
			if (map.collides(nextPosition(pos,mDirection,1)).empty()) {
				pos = nextPosition(pos, mDirection, 1);
			}
		}
	}

	if (score < 0) {
		score = 0;
	}

	if (score > high_score) {
		high_score = score;
	}
}

