#pragma once

#include <cassert>
#include <queue>
#include "GameConfig.h"

// incremental Dijkstra algorithm
class PathFinder
{
public:
	constexpr static size_t MAX_SIZE = 512;

	constexpr static int NEIGHBOR_X[] = { 0,0,1,-1,1,-1,1,-1 };
	constexpr static int NEIGHBOR_Y[] = { 1,-1,0,0,1,-1,-1,1 };

	constexpr static float SQRT_2 = 1.41421356237f;
	constexpr static float NEIGHBOR_DIST[] = { 1,1,1,1,SQRT_2,SQRT_2,SQRT_2,SQRT_2 };

	struct Cell
	{
		int x = 0;
		int y = 0;
		mutable size_t idx = 0;

		Cell() {};
		Cell(int x, int y) : x(x), y(y) {}
		Cell(int x, int y, int width) : x(x), y(y), idx(y* width + x) {}
	};

	struct Layer;
	struct Cmp
	{
		Layer* layer;

		inline bool operator() (const Cell& cell1, const Cell& cell2)
		{
			return layer->GetDist(cell1) > layer->GetDist(cell2);
		}
	};

	struct Layer
	{
		PathFinder* pathFinder;
		//size_t* visited;
		//float* dist;

		size_t	visited[MAX_SIZE * MAX_SIZE] = {};
		float	dist[MAX_SIZE * MAX_SIZE] = {};
		Cell	tempPrev[MAX_SIZE * MAX_SIZE] = {};

		size_t minIteration = 0;
		std::priority_queue<Cell, std::vector<Cell>, Cmp> queue;

		Layer() {};

		inline void Initialize(PathFinder* finder)
		{
			pathFinder = (finder);
			//visited = (finder->m_visited);
			//dist = (finder->m_dist);
			queue = { Cmp{ this }, std::vector<Cell>(8 * KB) };
			while (!queue.empty()) { queue.pop(); }
		}

		inline float GetDist(const Cell& cell)
		{
			return visited[cell.idx] < minIteration ?
				std::numeric_limits<std::remove_reference_t<decltype(dist[0])>>::max() : dist[cell.idx];
		}

		inline void Clear()
		{
			while (!queue.empty()) { queue.pop(); }
			minIteration = 0;
		}

		inline bool IsFinished() const
		{
			return minIteration == 0;
		}
	};

	//size_t m_minIteration = 0;
	size_t m_width = 0;
	size_t m_height = 0;

	bool* m_movable = nullptr;

	// iteration that path finder reach the call on map
	size_t	m_visited	[MAX_SIZE * MAX_SIZE] = {};
	//float	m_dist		[MAX_SIZE * MAX_SIZE] = {};
	//Cell	m_tempPrev	[MAX_SIZE * MAX_SIZE] = {};
	Cell	m_prev		[MAX_SIZE * MAX_SIZE] = {};

	//std::priority_queue<Cell, std::vector<Cell>, Cmp> m_queue;

	std::vector<Layer> m_layers;
	std::deque<Layer*> m_activeLayers;
	std::vector<Layer*> m_freeLayers;

	PathFinder() : m_activeLayers(16) //: m_queue(Cmp{ this }, std::vector<Cell>(8*KB))
	{
		/*while (!m_queue.empty())
		{
			m_queue.pop();
		}*/

		m_layers.resize(16);
		m_freeLayers.reserve(16);
		for (auto& layer : m_layers)
		{
			layer.Initialize(this);
			m_freeLayers.push_back(&layer);
		}
		m_activeLayers.clear();
	}

	inline auto& NextLayer()
	{
		Layer* ret = 0;
		if (!m_freeLayers.empty())
		{
			ret = m_freeLayers.back();
			m_freeLayers.pop_back();
		}
		else
		{
			ret = m_activeLayers.front();
			m_activeLayers.pop_front();
		}

		m_activeLayers.push_back(ret);
		return *ret;
	}

	inline void Initialize(bool* movable, size_t w, size_t h)
	{
		m_movable = movable;
		m_width = w;
		m_height = h;
	}

	inline Layer* Find(size_t iterationCount, const Cell& startCell)
	{
		startCell.idx = startCell.y * m_width + startCell.x;

		if (!m_movable[startCell.idx])
		{
			//m_visited[startCell.idx] = m_minIteration;
			return nullptr;
		}

		auto& layer = NextLayer();
		layer.Clear();

		layer.visited[startCell.idx] = 0;
		layer.dist[startCell.idx] = 0;
		m_prev[startCell.idx] = startCell;
		layer.tempPrev[startCell.idx] = startCell;

		layer.minIteration = iterationCount;
		layer.queue.push(startCell);
		return &layer;
		//m_queue.push(startCell);
	}

	// return true if done algorithm
	inline bool UpdateLayer(Layer* pLayer, size_t iterationCount, size_t step)
	{
		auto& layer = *pLayer;

		iterationCount++;
		assert(iterationCount > layer.minIteration);

		auto& queue = layer.queue;
		while (!queue.empty() && step != 0)
		{
			auto cur = queue.top();
			queue.pop();

			if (layer.visited[cur.idx] > layer.minIteration)
			{
				continue;
			}

			step--;
			layer.visited[cur.idx] = iterationCount;

			if (layer.minIteration > m_visited[cur.idx])
			{
				m_prev[cur.idx] = layer.tempPrev[cur.idx];
				m_visited[cur.idx] = layer.minIteration;
			}

			auto curDist = layer.GetDist(cur);

			for (size_t i = 0; i < sizeof(NEIGHBOR_X) / sizeof(NEIGHBOR_X[0]); i++)
			{
				Cell cell{ cur.x + NEIGHBOR_X[i], cur.y + NEIGHBOR_Y[i], (int)m_width };
				if (   !m_movable[cell.idx]
					|| cell.x < 0 || cell.x >= m_width 
					|| cell.y < 0 || cell.y >= m_height)
				{
					continue;
				}

				auto altDist = curDist + NEIGHBOR_DIST[i];
				auto cellDist = layer.GetDist(cell);
				if (altDist < cellDist)
				{
					layer.visited[cell.idx] = layer.minIteration;
					layer.tempPrev[cell.idx] = cur;
					layer.dist[cell.idx] = altDist;
					queue.push(cell);
				}
			}
		}

		//std::cout << m_queue.size() << "\n";
		if (queue.empty())
		{
			layer.minIteration = 0;
			m_freeLayers.push_back(&layer);
			return true;
		}

		return false;
	}

	inline void Update(size_t iterationCount, size_t step)
	{
		for (auto& layer : m_activeLayers)
		{
			UpdateLayer(layer, iterationCount, step);
		}
	}

public:
	inline Vec2 GetDir(const Vec2& pos) const
	{
		auto x = (size_t)std::floor(pos.x / (float)GameConfig::CELL_SIZE);
		auto y = (size_t)std::floor(pos.y / (float)GameConfig::CELL_SIZE);
		auto& prev = m_prev[y * m_width + x];
		Vec2 dir = Vec2((float)prev.x - (float)x, (float)prev.y - (float)y);
		if (dir.x == 0 && dir.y == 0)
		{
			return dir;
		}
		return dir.Normalize();
	}

};