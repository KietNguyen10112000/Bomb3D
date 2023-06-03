#pragma once

#include <cassert>
#include <queue>

// incremental Dijkstra algorithm
class PathFinder
{
public:
	constexpr static size_t MAX_SIZE = 512;

	struct Cell
	{
		int x = 0;
		int y = 0;
		mutable size_t idx = 0;

		Cell() {};
		Cell(int x, int y) : x(x), y(y)
		{

		}

		Cell(int x, int y, int width) : x(x), y(y), idx(y* width + x)
		{

		}
	};

	constexpr static int NEIGHBOR_X[] = { 0,0,1,-1 };
	constexpr static int NEIGHBOR_Y[] = { 1,-1,0,0 };

	struct Cmp
	{
		PathFinder* finder;

		inline bool operator() (const Cell& cell1, const Cell& cell2)
		{
			return finder->GetDist(cell1) < finder->GetDist(cell2);
		}
	};

	size_t m_minIteration = 0;
	size_t m_width = 0;
	size_t m_height = 0;

	bool* m_movable = nullptr;

	// iteration that path finder reach the call on map
	size_t	m_visited	[MAX_SIZE * MAX_SIZE] = {};
	size_t	m_dist		[MAX_SIZE * MAX_SIZE] = {};
	Cell	m_prev		[MAX_SIZE * MAX_SIZE] = {};

	std::priority_queue<Cell, std::vector<Cell>, Cmp> m_queue;

	PathFinder() : m_queue(Cmp{ this }, std::vector<Cell>(1024))
	{
	}

	inline void Setup(size_t iterationCount, bool* movable, size_t w, size_t h, const Cell& startCell)
	{
		m_movable = movable;
		m_minIteration = iterationCount;
		m_width = w;
		m_height = h;

		startCell.idx = startCell.y * m_width + startCell.x;

		m_visited[startCell.idx] = iterationCount;
		m_dist[startCell.idx] = 0;
		m_prev[startCell.idx] = startCell;

		m_queue.push(startCell);
	}

	// return true if done algorithm
	inline bool Find(size_t iterationCount, size_t step)
	{
		assert(iterationCount >= m_minIteration);

		while (!m_queue.empty() && (step--) != 0)
		{
			auto cur = m_queue.top();
			m_queue.pop();

			auto curDist = GetDist(cur);

			for (size_t i = 0; i < sizeof(NEIGHBOR_X) / sizeof(NEIGHBOR_X[0]); i++)
			{
				Cell cell{ cur.x + NEIGHBOR_X[i], cur.y + NEIGHBOR_Y[i], (int)m_width };
				if (   !m_movable[cell.idx]
					|| cell.x < 0 || cell.x >= m_width 
					|| cell.y < 0 || cell.y >= m_height)
				{
					continue;
				}

				auto altDist = curDist + 1;
				auto cellDist = GetDist(cell);
				if (altDist < cellDist)
				{
					m_visited[cell.idx] = iterationCount;
					m_prev[cell.idx] = cell;
					m_dist[cell.idx] = altDist;
					m_queue.push(cell);
				}
			}
		}

		if (m_queue.empty())
		{
			return true;
		}

		return false;
	}

	inline size_t GetDist(const Cell& cell)
	{
		return m_visited[cell.idx] < m_minIteration ? (size_t)(-1) : m_dist[cell.idx];
	}

};