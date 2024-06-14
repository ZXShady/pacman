
#include "headers/Game.hpp"

#include <cstdio>
// fix linker error for msvc only
#if defined(_MSC_VER) && !defined(__clang__)
::std::FILE _iob[] = { *stdin,*stdout,*stderr };
extern "C" ::std::FILE* __iob_func(void)
{
    return _iob;
}
#endif

int main() {
    pacman::Game game;
    game.run();
}