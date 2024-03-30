#include "headers/GhostManager.hpp"

void pacman::GhostManager::update(const Map& map, sf::Vector2i pacman_pos, Direction pacman_direction)
{
	setTargets(map, pacman_pos, pacman_direction);
	mBlinky.update(map);
	mPinky.update(map);
	mInky.update(map);
	mClyde.update(map);
	tickTimers();
}

void pacman::GhostManager::setTargets(const Map& map, sf::Vector2i pacman_pos, Direction pacman_direction)
{
	if (pacman_direction == Direction::None)
		pacman_direction = Direction::Up;

	mBlinky.setTarget(pacman_pos, map);
	mInky.setTarget(pacman_pos, pacman_direction, mBlinky.pos);
	mPinky.setTarget(pacman_pos, pacman_direction);
	mClyde.setTarget(pacman_pos, pacman_direction);
}

void pacman::GhostManager::loadTextures(const TextureManager& manager)
{
	for (auto ghost : get())
		ghost->loadTextures(manager);
}

void pacman::GhostManager::init(Map& map)
{
	mPhaseChanged = true;
	mMap = &map;
	mClyde.id = EntityID::Clyde;
	mBlinky.id = EntityID::Blinky;
	mInky.id = EntityID::Inky;
	mPinky.id = EntityID::Pinky;
}

void pacman::GhostManager::nextPhase()
{
}

void pacman::GhostManager::tickTimers()
{
	bool first = mTimeUntilNextPhase == 0;
	--mTimeUntilNextPhase;
	auto& phase = getPhase(mMap->data().level);
	auto& mode = phase.mode[mCurrentPhaseIndex];
	mCurrentMode = mode.type;
	if (mTimeUntilNextPhase <= 0) {
		mTimeUntilNextPhase = mode.duration;
		mPhaseChanged = true;
		mCurrentPhaseIndex++;
		if (mCurrentPhaseIndex >= 9) {
			mCurrentPhaseIndex = 0;
		}
	}

	bool fright = false;
	for (auto ghost : get()) {
		if (ghost->mFrightenedTimer) {
			fright = true;
			break;
		}
	}

	if (!fright && mPhaseChanged) {
		for (auto ghost : get()) {
			ghost->setMode(mCurrentMode);
		}
		mPhaseChanged = false;
	}
	if (first) {
		for (auto ghost : get()) {
			ghost->useDoor();
		}
	}
}


void pacman::GhostManager::draw(sf::RenderTarget& target)
{
	for (Ghost* ghost : get())
		ghost->draw(target);
}

void pacman::GhostManager::reset(typename const Map::Data& data)
{
	mBlinky.reset(data);
	mPinky.reset(data);
	mInky.reset(data);
	mClyde.reset(data);

	mTimeUntilNextPhase = 0;
	mCurrentPhaseIndex = 0;
	mPhaseChanged = true;
}
