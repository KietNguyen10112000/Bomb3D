#pragma once

#include "Core/TypeDef.h"

#include "Core/Random/Random.h"

using namespace soft;

class GameMgr
{
public:
	inline void GenerateMap(
		byte* mapValues, uint32_t& outputWidth, uint32_t& outputHeight,
		byte* blockedCells, uint32_t& outputBlockedCellCount)
	{
		blockedCells[0] = 0;
		blockedCells[1] = 2;
		outputBlockedCellCount = 2;

		size_t height = 32;
		size_t width = 32;
		outputWidth = width;
		outputHeight = height;
		for (size_t y = 0; y < height; y++)
		{
			auto row = &mapValues[y * width];
			for (size_t x = 0; x < width; x++)
			{
				row[x] = 1;// Random::RangeInt64(1, 2);

				if (y == 0 || y == height - 1 || x == 0 || x == width - 1)
				{
					row[x] = 0;
					continue;
				}

				auto v = Random::RangeInt64(1, 10);
				if (v == 10)
				{
					row[x] = 2;
				}
			}
		}

		for (size_t i = 0; i < 10; i++)
		{
			mapValues[3 * width + i + 3] = 1;
		}
	}

	inline void SetPlayerPos(ID id, Vec2& outputPos)
	{
		outputPos.x = (3 + id) * 60.0f;
		outputPos.y = 3 * 60.0f;
	}
};