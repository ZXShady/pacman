#include "headers/Map.hpp"
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

using namespace pacman;

void Map::loadTextures(const TextureManager& manager)
{
	mSprite->setTexture(manager.get(TextureID::Map));
}

void Map::tickTimers()
{
	++mEnergizerFlashingAnimation;
	if (mEnergizerFlashingAnimation >= kMaxEnergizerFlashingAnimation) {
		mEnergizerFlashingAnimation = 0;
	}
}

void Map::createMapImageFromASCII(const std::string& map_name, const std::vector<std::string>& ascii_sketch)
{
	sf::Image map_sketch;
	const sf::Vector2<std::size_t> ascii_size{ ascii_sketch[0].size(),ascii_sketch.size(), };
	int x = 0;
	int y = 0;


	const std::string path = "res/data/maps/" + map_name + ".png";

	map_sketch.loadFromFile(path);
	if (map_sketch.getSize().x > ascii_size.x
		|| map_sketch.getSize().y > ascii_size.y)
		assert(!"map sketch is bigger than the image.");

	for (const auto& s : ascii_sketch) {
		for (auto c : s) {
			map_sketch.setPixel(x, y, CellColors::getPixelColorFromChar(c));
			x++;
		}
		x = 0;
		y++;
	}
	map_sketch.saveToFile(path);

}

void pacman::Map::draw(sf::RenderTarget& target) noexcept
{
	for (const Cell& cell : mCells) {
		if (cell.type != CellType::Empty) {
			mSprite->setPosition(static_cast<float>(cell.pos.x), static_cast<float>(cell.pos.y));
			mSprite->setColor(cell.color);
			if (cell.type != CellType::Wall) {
				mSprite->setTextureRect(cell.getIntRect());
			}
			else {
				Cell* wall = getCell(cell.pos.x, cell.pos.y + kCellSize);
				bool down{ wall ? wall->type == CellType::Wall : false };
				wall = getCell(cell.pos.x, cell.pos.y - kCellSize);
				bool up{ wall ? wall->type == CellType::Wall : false };
				wall = getCell(cell.pos.x - kCellSize, cell.pos.y);
				bool left{ wall ? wall->type == CellType::Wall : false };
				wall = getCell(cell.pos.x + kCellSize, cell.pos.y);
				bool right{ wall ? wall->type == CellType::Wall : false };
				mSprite->setTextureRect(sf::IntRect(kCellSize * (down + 2 * (left + 2 * (right + 2 * up))), 0, kCellSize, kCellSize));

			}

			if (cell.type == CellType::Energizer) {
				if (mEnergizerFlashingAnimation < 0.45_sec) {
					target.draw(*mSprite);
				}
			}
			else {
				target.draw(*mSprite);
			}
		}
	}
}

std::vector<Cell*> Map::intersects(sf::Vector2i pos)
{
	std::vector<Cell*> collided_cells;
	float cell_x = pos.x / static_cast<float>(kCellSize);
	float cell_y = pos.y / static_cast<float>(kCellSize);

	for (int i = 0; i < 4; ++i) {
		int x = 0;
		int y = 0;

		switch (i) {
			case 0: //Top left cell
				x = static_cast<int>(std::floor(cell_x));
				y = static_cast<int>(std::floor(cell_y));
				break;
			case 1: //Top right cell
				x = static_cast<int>(std::ceil(cell_x));
				y = static_cast<int>(std::floor(cell_y));
				break;
			case 2: //Bottom left cell
				x = static_cast<int>(std::floor(cell_x));
				y = static_cast<int>(std::ceil(cell_y));
				break;
			case 3: //Bottom right cell
				x = static_cast<int>(std::ceil(cell_x));
				y = static_cast<int>(std::ceil(cell_y));
				break;
		}

		Cell* current_cell = getCell(x * kCellSize, y * kCellSize);
		if (current_cell)
			collided_cells.push_back(current_cell);
	}
	return collided_cells;
}

std::vector<Cell*> Map::collides(sf::Vector2i pos)
{
	auto c = intersects(pos);
	std::vector<Cell*> ret;
	ret.reserve(c.size());
	for (Cell* cell : c) {
		if (cell->hasCollision()) {
			ret.push_back(cell);
		}
	}
	return ret;
}

std::vector<const Cell*> Map::collides(sf::Vector2i pos) const
{
	auto c = intersects(pos);
	std::vector<const Cell*> ret;
	ret.reserve(c.size());
	for (const Cell* cell : c) {
		if (cell->hasCollision()) {
			ret.push_back(cell);
		}
	}
	return ret;
}

void Map::loadSketch(const std::vector<std::string>& sketch)
{
	mSketch = &sketch;
	mCells.clear();
	data.cell_count.clear();
	data.glass.clear();
	int x = 0;
	int y = 0;
	Cell current_cell;

	for (const auto& cells : sketch) {
		for (auto c : cells) {
			current_cell.type = CellType::Empty;
			switch (c) {
				case '#':
					current_cell.type = CellType::Wall;
					break;
				case '.':
					current_cell.type = CellType::Pellet;
					break;
				case '=':
					current_cell.type = CellType::Glass;
					data.glass.push_back(current_cell);
					break;
				case 'E':
					current_cell.type = CellType::Wall;
					data.exit_pos = { x,y + kCellSize };
					break;
				case 'o':
					current_cell.type = CellType::Energizer;
					break;
				case 'i':
					data.entities[EntityID::Inky].starting_pos = { x,y };
					break;
				case 'b':
					data.entities[EntityID::Blinky].starting_pos = { x,y };
					break;
				case 'p':
					data.entities[EntityID::Pinky].starting_pos = { x,y };
					break;
				case 'c':
					data.entities[EntityID::Clyde].starting_pos = { x,y };
					break;
				case 'K':
					data.pacman_pos = { x,y };
					break;
				case 'I':
					data.entities[EntityID::Inky].scatter_pos = { x,y };
					break;
				case 'B':
					data.entities[EntityID::Blinky].scatter_pos = { x,y };
					break;
				case 'P':
					data.entities[EntityID::Pinky].scatter_pos = { x,y };
					break;
				case 'C':
					data.entities[EntityID::Clyde].scatter_pos = { x,y };
					break;
				case ' ':
					current_cell.type = CellType::Empty;
					break;
				default:
					assert(!"found undefined cell type");
			}
			current_cell.pos = { x,y };
			data.cell_count[current_cell.type]++;
			mCells.push_back(current_cell);


			x += kCellSize;
		}
		y += kCellSize;
		x = 0;
	}
}