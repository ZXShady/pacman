#include "headers/Map.hpp"

void pacman::Map::loadTextures(const TextureManager& manager)
{
	mSprite->setTexture(manager.get(TextureID::Map));
}
