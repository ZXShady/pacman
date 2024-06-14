#ifndef PACMAN_MAP_HPP
#define PACMAN_MAP_HPP

#include <cmath>
#include <string>
#include <vector>

#include "Globals.hpp"
#include "ResourceManager.hpp"
#include <SFML/Graphics/Sprite.hpp>
#include <memory>
namespace pacman {


class Map {
public:
	struct Data {
		struct value {
			sf::Vector2i scatter_pos;
			sf::Vector2i starting_pos;
		};
		struct portal {
			int id;
			std::vector<Cell*> links;
		};

		std::vector<portal> portals;
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
	void tickTimers();
	void update()
	{
	}

	void createMapImageFromASCII(const std::string& map_name, const std::vector<std::string>& ascii_sketch);

	void loadSketch(const std::vector<std::string>& sketch);


	void draw(sf::RenderTarget& target) noexcept;

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

	std::vector<Cell*> intersects(sf::Vector2i pos);
	std::vector<const Cell*> intersects(sf::Vector2i pos) const
	{
		const auto c = const_cast<Map*>(this)->intersects(pos);
		return std::vector<const Cell*>{c.begin(), c.end()};
	}



	std::vector<Cell*> collides(sf::Vector2i pos);
	std::vector<const Cell*> collides(sf::Vector2i pos) const;
	
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
		data.portals.clear();
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
