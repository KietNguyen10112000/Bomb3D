#pragma once

#include "Core/TypeDef.h"

#include "Core/Random/Random.h"

#include "MazeGenerator.h"

using namespace soft;

class GameMgr
{
public:
	constexpr static size_t MAP_SIZE = 6;

	inline void GenerateMonsters(MatchStartAction* action, byte* map, size_t width, size_t height)
	{
		auto& monsters = action->m_mapMonstersId;
		auto& items = action->m_mapItems;

		for (size_t y = 0; y < height; y++)
		{
			auto idy = y * width;
			auto row = &monsters[idy];
			auto rowItem = &items[idy];
			auto mapRow = &map[idy];
			for (size_t x = 0; x < width; x++)
			{
				row[x] = 255;
				auto movable = (bool)mapRow[x] ? false : true;
				if (rowItem[x] != 255 || !movable)
				{
					continue;
				}

				if (Random::RangeInt32(0, 5) == 0)
				{
					row[x] = 1;
				}
			}
		}

		// no monsters at players start location
		for (size_t y = 0; y < 10; y++)
		{
			auto idy = y * width;
			auto row = &monsters[idy];
			for (size_t x = 0; x < 10; x++)
			{
				row[x] = 255;
			}
		}

		for (size_t y = height - 10; y < height; y++)
		{
			auto idy = y * width;
			auto row = &monsters[idy];
			for (size_t x = width - 10; x < width; x++)
			{
				row[x] = 255;
			}
		}
	}

	inline void GenerateMap(
		MatchStartAction* action,
		byte* mapValues, uint32_t& outputWidth, uint32_t& outputHeight,
		byte* itemValues,
		byte* blockedCells, uint32_t& outputBlockedCellCount)
	{
		blockedCells[0] = 0;
		blockedCells[1] = 2;
		outputBlockedCellCount = 2;

		MazeGenerator generator;
		generator.Create(MAP_SIZE);

		auto map = generator.GetMap();

		size_t height = generator.Size();
		size_t width = generator.Size();
		outputWidth = width;
		outputHeight = height;
		for (size_t y = 0; y < height; y++)
		{
			auto row = &mapValues[y * width];
			auto items = &itemValues[y * width];
			auto mapRow = &map[y * width];
			for (size_t x = 0; x < width; x++)
			{
				auto movable = (bool)mapRow[x] ? false : true;
				row[x] = movable ? 1 : 2;// Random::RangeInt64(1, 2);

				if (y == 0 || y == height - 1 || x == 0 || x == width - 1)
				{
					row[x] = 0;
					//continue;
				}

				/*auto v = Random::RangeInt64(1, 10);
				if (v == 10)
				{
					row[x] = 2;
				}*/

				items[x] = 255;
				if (row[x] != 0 && Random::RangeInt32(0, 5) == 0)
				{
					auto v = Random::RangeInt32(0, 512);
					if (v <= 490)
					{
						// coin
						items[x] = 0;
					}
					else if (v <= 500)
					{
						// pistol
						items[x] = 1;
					}
					else if (v <= 510)
					{
						// smg
						items[x] = 2;
					}
				}
			}
		}

		for (size_t i = 0; i < 10; i++)
		{
			mapValues[3 * width + i + 3] = 1;
		}

		action->m_randomSeed = (size_t)Random::RangeInt64(std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max());

		GenerateMonsters(action, map, width, height);
	}

	inline void SetPlayerPos(ID id, Vec2& outputPos)
	{
		outputPos.x = (3 + id) * 60.0f;
		outputPos.y = 3 * 60.0f;
	}
};