#include "headers/Ghost.hpp"

void pacman::Ghost::reachedHome()
{
    mCanUseDoor = false;
    mMode = Mode::Chase;
    mFrightenedTimer = 0;
    mSpeed = kSpeedWhenChasing;
}

void pacman::Ghost::reachedSpawnPoint()
{
    mCanUseDoor = true;
    mMode = Mode::Chase;
    mTargetPos = mHomeExitPos;
    mFrightenedTimer = 0;
    mSpeed = kSpeedWhenChasing;
}

void pacman::Ghost::frighten()
{
    mCanUseDoor = false;
    mMode = Mode::Frightened;
    mFrightenedTimer = kFrightenedDuration;
    mSpeed = kSpeedWhenFrightened;
    mDirection = flipDirection(mDirection);
}


void pacman::Ghost::chase()
{
    mCanUseDoor = false;
    mMode = Mode::Chase;
    mSpeed = kSpeedWhenChasing;
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
    switch (mode) {
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
        case Mode::None:
            mSpeed = 1;
            break;
    }
}