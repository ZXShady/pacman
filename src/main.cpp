#include "headers/Game.hpp"

#include <cstdio>
// fix linker error
::std::FILE _iob[] = { *stdin,*stdout,*stderr };
extern "C" ::std::FILE* __iob_func(void)
{
    return _iob;
}

int main()
{
    pacman::Game game;
    game.run();
}