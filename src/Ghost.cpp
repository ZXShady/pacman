#include "headers/Ghost.hpp"

void pacman::Ghost::reachedHomeExit()
{
    mCanUseDoor = false;
    mFrightenedTimer = 0;
    setMode(Mode::None);
}

void pacman::Ghost::reachedHome()
{
    mCanUseDoor = true;
    mTargetPos = mHomeExitPos;
    setMode(Mode::ReachTarget);
    mFrightenedTimer = 0;
}

void pacman::Ghost::frighten()
{
    mCanUseDoor = false;
    mMode = Mode::Frightened;
    mFrightenedTimer = kFrightenedDuration;
    mDirection = flipDirection(mDirection);
}



void pacman::Ghost::chase()
{
    mCanUseDoor = false;
    mMode = Mode::Chase;
    mDirection = flipDirection(mDirection);
    mFrightenedTimer = 0;
}


void pacman::Ghost::scatter()
{
    mCanUseDoor = false;
    mTargetPos = mScatterPos;
    mMode = Mode::Scatter;
    mDirection = flipDirection(mDirection);
}
void pacman::Ghost::setMode(Mode mode)
{
    mMode = mode;
    switch (mMode) {
        case Mode::Eaten:
            die();
            break;
        case Mode::Frightened:
            frighten();
            break;
        case Mode::Chase:
            chase();
            break;
        case Mode::Scatter:
            scatter();
            break;
    }
    setSpeed(mode);
}

void pacman::Ghost::die()
{
    if (mMode == Mode::Eaten)
        return;

    mCanUseDoor = true;
    mTargetPos = mHomePos;
    mMode = Mode::Eaten;
    mFrightenedTimer = 0;
    mSpeed = kSpeedWhenEaten;
}

void pacman::Ghost::setSpeed(Mode mode)
{
    switch (mode) {
        case Mode::Eaten:
            mSpeed = kSpeedWhenEaten;
            break;
        case Mode::Scatter:
            mSpeed = kSpeedWhenScatter;
            break;
        case Mode::Chase:
            mSpeed = kSpeedWhenChasing;
            break;
        case Mode::Frightened:
            mSpeed = kSpeedWhenFrightened;
            break;
        case Mode::ReachTarget:
            mSpeed = kSpeedWhenReachingTarget;
            break;
        case Mode::None:
            mSpeed = 1;
            break;
    }
}

void pacman::Ghost::update(const Map& map)
{
    setSpeed(mMode);
    if (!map.intersectsIf(pos, [](const Cell& c) { return c.type == CellType::Glass; }).empty()) {
        mCanUseDoor = true;
    }

    if (-kCellSize >= pos.x)
        pos.x = kCellSize * kMapWidth - mSpeed;
    else if (kCellSize * kMapWidth - mSpeed <= pos.x)
        pos.x = mSpeed - kCellSize;

    if (-kCellSize >= pos.y)
        pos.y = kCellSize * kMapHeight - mSpeed;
    else if (kCellSize * kMapHeight - mSpeed <= pos.y)
        pos.y = mSpeed - kCellSize;

    for (int i=0; i < mSpeed; i++) {
        if (pos == mTargetPos) {
            if (mHomeExitPos == mTargetPos) {
                reachedHomeExit();
            }
            else if (mHomePos == mTargetPos) {
                reachedHome();
            }
        }






        auto wall = [&map, this](Direction dir) -> bool
            {
                return !map.intersectsIf(nextPosition(this->pos, dir, 1), [&map, this](Cell& c)
                    {
                        return this->mCanUseDoor
                            ? c.hasCollision()
                            : (c.type == CellType::Glass || c.hasCollision());
                    }).empty();
            };

        int ways = 0;
        if (mMode != Mode::Frightened) {
            Direction optimal_direction = Direction::None;

            for (auto dir : Directions) {
                if (dir == flipDirection(mDirection))
                    continue;

                if (!wall(dir)) {
                    if (optimal_direction == Direction::None) {
                        optimal_direction = dir;
                    }

                    ways++;

                    if (pythagoras(nextPosition(pos, dir, 1) - mTargetPos)
                        < pythagoras(nextPosition(pos, optimal_direction, 1) - mTargetPos)) {
                        optimal_direction = dir;
                    }
                }
            }

            if (ways > 1)
                mDirection = optimal_direction;
            else if (optimal_direction == Direction::None)
                mDirection = flipDirection(mDirection);
            else
                mDirection = optimal_direction;
        }
        else {
            Direction random_direction = randomDirection();

            for (auto dir : Directions) {
                if (dir == flipDirection(mDirection))
                    continue;

                if (!wall(dir))
                    ways++;
            }

            if (ways > 0) {
                while (wall(random_direction) || random_direction == flipDirection(mDirection))
                    random_direction = randomDirection();
                mDirection = random_direction;
            }
            else {
                mDirection = flipDirection(mDirection);
            }
        }
        if (map.canMove(nextPosition(pos, mDirection, 1))) {
            pos = nextPosition(pos, mDirection, 1);
        }
    }
}
