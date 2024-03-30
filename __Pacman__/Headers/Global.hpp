#pragma once
#include <SFML/System/Vector2.hpp>
//I won't explain this.
constexpr unsigned char CELL_SIZE = 16;
//This too.
constexpr unsigned char FONT_HEIGHT = 16;
//Okay, I'll explain this.
//I start counting everything from 0, so this is actually the second ghost.
//The website used smaller cells, so I'm setting smaller values.
//Which cell in front of Pacman the pink ghost will target.
constexpr auto GHOST_1_CHASE = 2;
//Which cell in front of Pacman the blue ghost will use to calculate the target.
constexpr auto GHOST_2_CHASE = 1;
//If the distance between the orange ghost and Pacman is less than this value, the orange ghost will go into the scatter mode.
constexpr auto GHOST_3_CHASE = 4;
//How many frames are in the ghost body animation
constexpr auto GHOST_ANIMATION_FRAMES = 6;
//What do you think?
constexpr auto GHOST_ANIMATION_SPEED = 4;
//The speed of the ghost after the Pacman touches it while being energized.
constexpr auto GHOST_ESCAPE_SPEED = 4;
//Since the normal speed of the ghost is 1, and I didn't like the idea of using floating numbers, I decided to move the ghost after this number of frames.
//So the higher the value, the slower the ghost.
constexpr auto GHOST_FRIGHTENED_SPEED = 3;
//I won't explain the rest. Bite me!
constexpr auto GHOST_SPEED = 1;
constexpr auto MAP_HEIGHT = 21;
constexpr auto MAP_WIDTH = 21;
constexpr auto PACMAN_ANIMATION_FRAMES = 6;
constexpr auto PACMAN_ANIMATION_SPEED = 4;
constexpr auto PACMAN_DEATH_FRAMES = 12;
constexpr auto PACMAN_SPEED = 2;
constexpr auto SCREEN_RESIZE = 2;

//This is in frames. So don't be surprised if the numbers are too big.
constexpr auto CHASE_DURATION = 1024;
constexpr auto ENERGIZER_DURATION = 512;
constexpr auto FRAME_DURATION = 16667;
constexpr auto GHOST_FLASH_START = 64;
constexpr auto LONG_SCATTER_DURATION = 512;
constexpr auto SHORT_SCATTER_DURATION = 256;

//I used enums! I rarely use them, so enjoy this historical moment.
enum Cell
{
	Door,
	Empty,
	Energizer,
	Pellet,
	Wall
};

enum Direction {
	Right,Up,Left,Down
};

using Position = sf::Vector2i;
