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

	Vec2 m_maxPos = {};

	// 1 - movable, 0 - unmovable
	bool m_movable[MAX_SIZE * MAX_SIZE] = {};

	Cell m_cells[MAX_SIZE * MAX_SIZE] = {};

	Item* m_items[256] = {};

	// each player contains 1 path finder
	PathFinder* m_playerPathFinders[GameConfig::MAX_PLAYERS] = {};

	// each team contains 1 path finder for victory tower
	PathFinder* m_victoryTowersPathFinder[GameConfig::MAX_PLAYERS] = {};

	~GameMap();

private:
	friend class GameLoopHandler;
	
	void Initialize(MatchStartAction* matchStart);
	void InitializePathFinders();
	void FinalizePathFinders();
	void InitializeItems();
	void FinalizeItems();

public:
	inline bool IsMovable(const Vec2& pos) const
	{
		auto x = (size_t)std::floor(pos.x / (float)GameConfig::CELL_SIZE);
		auto y = (size_t)std::floor(pos.y / (float)GameConfig::CELL_SIZE);

		return m_movable[y * m_width + x];
	}

	inline ID GetItemID(const Vec2& pos)
	{
		auto cell = m_playerPathFinders[0]->GetCell(pos);
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
		auto cell = m_playerPathFinders[0]->GetCell(pos);
		m_cells[cell.idx].itemId = ((byte)-1);
	}

	inline auto IsOutside(const Vec2& pos)
	{
		return pos.x < 0 || pos.y < 0 || pos.x > m_maxPos.x || pos.y > m_maxPos.y;
	}

	inline auto& GetPlayerPathFinder(ID userId)
	{
		return m_playerPathFinders[userId];
	}

	inline auto& GetTeamPathFinder(ID teamId)
	{
		return m_victoryTowersPathFinder[teamId];
	}
};