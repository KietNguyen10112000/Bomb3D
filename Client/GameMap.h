#pragma once
#include <cstdint>

#include "PathFinder.h"

class GameMap
{
public:
	constexpr static size_t MAX_SIZE = PathFinder::MAX_SIZE;

	size_t m_width = 0;
	size_t m_height = 0;

	// 1 - movable, 0 - unmovable
	bool m_movable[MAX_SIZE * MAX_SIZE] = {};

	PathFinder m_pathFinder;

	inline void Initialize(size_t w, size_t h, uint8_t* data, uint8_t* blockCells, uint8_t blockCellCount)
	{
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
				m_movable[x + yy] = !isCellBlocked[data[x + yy]];
			}
		}
	}

};