#include "headers/GhostManager.hpp"
#include <array>

void pacman::GhostManager::update(const Map& map, sf::Vector2i pacman_pos, Direction pacman_direction)
{
	setTargets(map, pacman_pos, pacman_direction);
	mBlinky.update(map);
	mPinky.update(map);
	mInky.update(map);
	mClyde.update(map);

	for (Ghost* ghost : get()) {
		const auto m = ghost->getMode();
		if (m != Ghost::Mode::Eaten
		&& m != Ghost::Mode::Frightened
		&& m != Ghost::Mode::ReachTarget
		&& m != mCurrentMode) {
			ghost->setMode(mCurrentMode);
		}
	}
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

void pacman::GhostManager::init(const Map& map)
{
	mMap = &map;
	mClyde.id = EntityID::Clyde;
	mBlinky.id = EntityID::Blinky;
	mInky.id = EntityID::Inky;
	mPinky.id = EntityID::Pinky;
}

void pacman::GhostManager::startNextPhase()
{

	const Phase* phase = nullptr;
	for (auto& p : kPhases) {
		if (in_minmax_range(mMap->data.level, p.level_range)) {
			phase = &p;
			break;
		}
	}
	assert(phase && "couldn't find phase with required level.");
	mTimeUntilNextPhase = phase->modes[mCurrentModeIndex].duration;
	mCurrentMode = phase->modes[mCurrentModeIndex].type;
	++mCurrentModeIndex;
	if (mCurrentModeIndex >= pacman::size(phase->modes)) {
		mCurrentModeIndex = 0;
	}

}

void pacman::GhostManager::tickTimers()
{
	--mTimeUntilNextPhase;

	if (mTimeUntilNextPhase <= 0) {
		startNextPhase();
	}
	--mTimeUntilGhostSiren;
	if (mTimeUntilGhostSiren==0) {
		mTimeUntilGhostSiren = std::max(20_sec,std::rand() % 30_sec);
		mGhostSirenSound.play();
	}
}


void pacman::GhostManager::draw(sf::RenderTarget& target)
{
	for (Ghost* ghost : get())
		ghost->draw(target);
}

void pacman::GhostManager::reset(const typename Map::Data& data)
{
	mBlinky.reset(data);
	mPinky.reset(data);
	mInky.reset(data);
	mClyde.reset(data);

	mTimeUntilNextPhase = 0;
	mCurrentModeIndex = 0;
	mTimeUntilGhostSiren = 3.5_sec;
	mCurrentMode = Ghost::Mode::None;
}
