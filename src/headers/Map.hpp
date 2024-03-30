#ifndef PACMAN_MAP_HPP
#define PACMAN_MAP_HPP

#include <cmath>
#include <string>
#include <vector>

#include "Globals.hpp"
#include "ResourceManager.hpp"

namespace pacman {


class Map {
public:
	struct Data {
		struct value {
			sf::Vector2i scatter_pos;
			sf::Vector2i starting_pos;
		};
		std::unordered_map<EntityID, value> entities;
		std::unordered_map<CellType, int> cell_count;
		std::vector<Cell> glass;
		sf::Vector2i exit_pos;
		sf::Vector2i pacman_pos;
		int level{ 1 };
	};

	struct CellColors {
		CellColors() = delete;

		static sf::Color Wall() { return sf::Color{ 0x0000FF }; }
		static sf::Color Pellet() { return sf::Color::Black; }
		static sf::Color Energizer() { return sf::Color{ 0xFF7E00 }; }
		static sf::Color Glass() { return sf::Color{ 0x00FFEB }; }
		static sf::Color Pacman() { return sf::Color{ 0xFFFB00 }; }
		static sf::Color PinkGhost() { return sf::Color{ 0xFFC0CB }; }
		static sf::Color OrangeGhost() { return sf::Color{ 0xFFA500 }; }
		static sf::Color CyanGhost() { return sf::Color{ 0x00FFFF }; }
		static sf::Color RedGhost() { return sf::Color(0xFF0000); }
		static sf::Color Empty() { return sf::Color::White; }

		static sf::Color getPixelColorFromChar(char c)
		{
			switch (c) {
				case '#': return Wall();
				case '.': return Pellet();
				case '=': return Glass();
				case 'o': return Energizer();
				case 'i': return CyanGhost();
				case 'b': return RedGhost();
				case 'p': return PinkGhost();
				case 'c': return OrangeGhost();
				case 'P': return Pacman();
			}
			return Empty();
		}
	};

	using type = ::std::vector<Cell>;
	using iterator = typename type::iterator;
	using const_iterator = typename type::const_iterator;
	using reverse_iterator = typename type::reverse_iterator;
	using const_reverse_iterator = typename type::const_reverse_iterator;

	void loadTextures(const TextureManager& manager);
	void update()
	{
		++mEnergizerFlashingAnimation;
		if (mEnergizerFlashingAnimation >= kMaxEnergizerFlashingAnimation) {
			mEnergizerFlashingAnimation = 0;
		}
	}

	void createMapImageFromASCII(const std::string& map_name, const std::vector<std::string>& ascii_sketch)
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

	void loadSketch(const std::vector<std::string>& sketch) noexcept
	{
		mSketch = &sketch;
		mCells.clear();
		data.cell_count.clear();
		data.glass.clear();
		int x = 0;
		int y = 0;
		Cell current_cell;

		for (const auto& cells : sketch) {
			for (auto cell : cells) {
				current_cell.type = CellType::Empty;

				switch (cell) {
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


	void draw(sf::RenderTarget& target) noexcept
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

	template<typename Pred>
	std::vector<Cell*> intersectsIf(sf::Vector2i pos, Pred&& pred) noexcept
	{
		auto v = intersects(pos);
		std::vector<Cell*> ret;
		for (auto c : v) {
			if (static_cast<Pred&&>(pred)(*c)) {
				ret.push_back(c);
			}
		}
		return ret;
	}

	template<typename Pred>
	std::vector<const Cell*> intersectsIf(sf::Vector2i pos, Pred&& pred) const noexcept
	{
		auto c = const_cast<Map*>(this)->intersectsIf(pos, std::forward<Pred>(pred));
		return std::vector<const Cell*>{c.begin(), c.end()};
	}

	std::vector<Cell*> intersects(sf::Vector2i pos) noexcept
	{
		std::vector<Cell*> collided_cells;
		float cell_x = pos.x / static_cast<float>(kCellSize);
		float cell_y = pos.y / static_cast<float>(kCellSize);

		for (int a = 0; a < 4; a++) {
			int x = 0;
			int y = 0;

			switch (a) {
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
	std::vector<const Cell*> intersects(sf::Vector2i pos) const noexcept
	{
		const auto c = const_cast<Map*>(this)->intersects(pos);
		return std::vector<const Cell*>{c.begin(), c.end()};
	}



	std::vector<Cell*> collides(sf::Vector2i pos) noexcept
	{
		const auto v = intersects(pos);
		std::vector<Cell*> ret;
		for (Cell* c : v) {
			if (c->hasCollision()) {
				ret.push_back(c);
			}
		}
		return ret;
	}

	std::vector<const Cell*> collides(sf::Vector2i pos) const noexcept
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
	type& vector() noexcept { return mCells; }
	const type& vector() const noexcept { return mCells; }


	Cell* getCell(sf::Vector2i pos) noexcept
	{
		return getCell(pos.x, pos.y);
	}

	const Cell* getCell(sf::Vector2i pos) const noexcept
	{
		return getCell(pos.x, pos.y);
	}

	Cell* getCell(int x, int y) noexcept
	{
		return const_cast<Cell*>(static_cast<const Map*>(this)->getCell(x, y));
	}

	const Cell* getCell(int x, int y) const noexcept
	{
		const sf::Vector2i pos{ x,y };
		for (const auto& cell : mCells) {
			if (cell.pos == pos) {
				return &cell;
			}
		}
		return nullptr;
	}

	static bool isOutOfBounds(sf::Vector2i pos) noexcept
	{
		return isOutOfBounds(pos.x, pos.y);
	}

	static bool isOutOfBounds(int x, int y) noexcept
	{
		return x > kCellSize * kMapWidth
			|| y > kCellSize * kMapHeight
			|| x < 0
			|| y < 0;
	}

	bool canMove(sf::Vector2i pos) const noexcept
	{
		return collides(pos).empty();
	}
	typename type::iterator begin() noexcept
	{
		return mCells.begin();
	}

	typename type::iterator end() noexcept
	{
		return mCells.end();
	}

	typename type::const_iterator begin() const noexcept
	{
		return mCells.begin();
	}

	typename type::const_iterator end() const noexcept
	{
		return mCells.end();
	}

	void reset()
	{
		mCells.clear();
		data.cell_count.clear();
		data.glass.clear();
	}

	int countCells(CellType type) const noexcept
	{
		int count = 0;
		for (const auto& c : mCells)
			count += c.type == type;
		return count;
	}
public:
	Data data;
private:
	const std::vector<std::string>* mSketch;
	std::unique_ptr<sf::Sprite> mSprite{ new sf::Sprite };
	std::vector<Cell> mCells;
	int mEnergizerFlashingAnimation{ 0 };
	static constexpr int kMaxEnergizerFlashingAnimation = 0.6_sec;


};

}
#endif // !PACMAN_MAP_HPP
