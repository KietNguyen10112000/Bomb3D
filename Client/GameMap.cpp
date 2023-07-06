#include "GameMap.h"
#include "Item.h"

#include "Global.h"

GameMap::~GameMap()
{
	FinalizeItems();
	FinalizePathFinders();
}

void GameMap::Initialize(MatchStartAction* matchStart)
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
	m_maxPos.x = (float)w * GameConfig::CELL_SIZE;
	m_maxPos.y = (float)h * GameConfig::CELL_SIZE;

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

	//m_pathFinder.Initialize(m_movable, w, h);

	InitializePathFinders();

	InitializeItems();
}

void GameMap::InitializePathFinders()
{
	auto count = Global::Get().activePlayerCount;
	for (size_t i = 0; i < count; i++)
	{
		auto& pathFinder = m_playerPathFinders[i];
		pathFinder = new PathFinder();
		pathFinder->Initialize(m_movable, m_width, m_height);
	}

	count = Global::Get().activeTeamCount;
	for (size_t i = 0; i < count; i++)
	{
		auto& pathFinder = m_victoryTowersPathFinder[i];
		pathFinder = new PathFinder();
		pathFinder->Initialize(m_movable, m_width, m_height);
	}
}

void GameMap::FinalizePathFinders()
{
	for (auto& e : m_playerPathFinders)
	{
		if (e) delete e;
	}

	for (auto& e : m_victoryTowersPathFinder)
	{
		if (e) delete e;
	}
}

void GameMap::InitializeItems()
{
	m_items[0] = new Coin();
	//m_items[1] = new Coin();
	//m_items[1] = new Coin();
}

void GameMap::FinalizeItems()
{
	for (auto& item : m_items)
	{
		if (item) delete item;
	}
}

