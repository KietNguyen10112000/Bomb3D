#pragma once
#include <cstdint>

#include "PathFinder.h"

#include "GameActions/MatchStartAction.h"

class Item;

class GameMap
{
public:
	struct Cell
	{
		int value;
		int itemId;
	};

	constexpr static size_t MAX_SIZE = PathFinder::MAX_SIZE;

	size_t m_width = 0;
	size_t m_height = 0;

	// 1 - movable, 0 - unmovable
	bool m_movable[MAX_SIZE * MAX_SIZE] = {};

	Cell m_cells[MAX_SIZE * MAX_SIZE] = {};

	Item* m_items[256] = {};

	PathFinder m_pathFinder;

	~GameMap();

	void Initialize(MatchStartAction* matchStart);
	void InitializeItems();
	void FinalizeItems();

	inline bool IsMovable(const Vec2& pos) const
	{
		auto x = (size_t)std::floor(pos.x / (float)GameConfig::CELL_SIZE);
		auto y = (size_t)std::floor(pos.y / (float)GameConfig::CELL_SIZE);

		return m_movable[y * m_width + x];
	}

	inline ID GetItemID(const Vec2& pos)
	{
		auto cell = m_pathFinder.GetCell(pos);
		if (m_movable[cell.idx] && m_cells[cell.idx].itemId != ((byte)-1))
			return m_cells[cell.idx].itemId;

		return INVALID_ID;
	}

	inline Item* GetItem(const Vec2& pos)
	{
		auto id = GetItemID(pos);
		if (id == INVALID_ID)
		{
			return nullptr;
		}
		return m_items[id];
	}

	inline void ClearItem(const Vec2& pos)
	{
		auto cell = m_pathFinder.GetCell(pos);
		m_cells[cell.idx].itemId = ((byte)-1);
	}
};