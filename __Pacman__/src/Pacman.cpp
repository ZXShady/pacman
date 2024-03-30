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

void pacman::Pacman::update(GhostManager& ghosts, Map& map)
{
	if (!isDead()) {
		for (Ghost* ghost : ghosts.get()) {
			//printf("ghost %p           %d \n", (void*)ghost,ghost->color);
			if (collidesWithEntity(ghost->pos)) {
				if (isEnergized() && ghost->getMode() == Ghost::Mode::Frightened) {
					ghost->die();
					mEatGhostSound.play();
				}
				else if (!isDead() && ghost->getMode() != Ghost::Mode::Eaten) {
					die();
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
			&& canMove(Direction::Up, map))
			dir = Direction::Up;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)
			&& canMove(Direction::Down, map))
			dir = Direction::Down;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)
			&& canMove(Direction::Left, map))
			dir = Direction::Left;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)
			&& canMove(Direction::Right, map))
			dir = Direction::Right;

		if (dir != Direction::None)
			mDirection = dir;

		for (Cell* cell : map.intersects(nextPosition(pos, mDirection, mSpeed))) {
			if (cell->isEdible()) {
				if (cell->type == CellType::Energizer) {
					mEnergizerTimer = std::max(4_sec, kEnergizerDuration - (10 * map.data().level - 1));
					mEnergizerSound.play();
					for (Ghost* ghost : ghosts.get())
						ghost->frighten();
				}


				cell->type = CellType::Empty;
				++mEatenPellets;
				if (mEatenPellets % 2)
					mWaSound.play();
				else
					mKaSound.play();
			}
		}
		if (canMove(mDirection, map)) {
			pos = nextPosition(pos, mDirection, mSpeed);
		}
	}
	tickTimers();
}

