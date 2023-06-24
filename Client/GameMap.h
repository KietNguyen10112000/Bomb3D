#pragma once
#include <cstdint>

#include "PathFinder.h"

#include "GameActions/MatchStartAction.h"

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

	PathFinder m_pathFinder;

	inline void Initialize(MatchStartAction* matchStart)
	{
		auto blockCellCount = matchStart->m_numBlockCell;
		auto blockCells = matchStart->m_blockCellValues;
		auto w = matchStart->m_width;
		auto h = matchStart->m_height;
		auto mapValues = matchStart->m_map;
		auto itemIds = matchStart->m_mapItems;

		bool isCellBlocked[256] = {};
		for (size_t i = 0; i < blockCellCount; i++)
		{
			isCellBlocked[blockCells[i]] = true;
		}

		m_width = w;
		m_height = h;
		for (size_t y = 0; y < h; y++)
		{
			auto yy = y * w;
			for (size_t x = 0; x < w; x++)
			{
				m_movable[x + yy] = !isCellBlocked[mapValues[x + yy]];
			}
		}

		for (size_t y = 0; y < h; y++)
		{
			auto yy = y * w;
			for (size_t x = 0; x < w; x++)
			{
				auto& cell = m_cells[x + yy];
				cell.value = mapValues[x + yy];
				cell.itemId = itemIds[x + yy];
			}
		}

		m_pathFinder.Initialize(m_movable, w, h);
	}

	inline bool IsMovable(const Vec2& pos) const
	{
		auto x = (size_t)std::floor(pos.x / (float)GameConfig::CELL_SIZE);
		auto y = (size_t)std::floor(pos.y / (float)GameConfig::CELL_SIZE);

		return m_movable[y * m_width + x];
	}

};