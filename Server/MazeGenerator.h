#pragma once
#include <cstdint>
#include <memory.h>
#include <stdlib.h>

class MazeGenerator
{
public:
	enum directions { NONE, NOR = 1, EAS = 2, SOU = 4, WES = 8 };
	using BYTE = uint8_t;

	MazeGenerator()
	{
		_world = 0;
		m_map = 0;
	}

	~MazeGenerator() { killArray(); }

	void Create(int side)
	{
		if (!(side & 1)) side++;
		_s = side;
		generate();
		translateToMap();
	}

	inline auto GetMap()
	{
		return m_map;
	}


private:
	void generate()
	{
		killArray();
		_world = new BYTE[_s * _s];
		memset(_world, 0, _s * _s);
		_ptX = rand() % _s; _ptY = rand() % _s;
		carve();
	}

	void carve()
	{
		while (true)
		{
			int d = getDirection();
			if (d < NOR) return;

			switch (d)
			{
			case NOR:
				_world[_ptX + _s * _ptY] |= NOR; _ptY--;
				_world[_ptX + _s * _ptY] = SOU | SOU << 4;
				break;
			case EAS:
				_world[_ptX + _s * _ptY] |= EAS; _ptX++;
				_world[_ptX + _s * _ptY] = WES | WES << 4;
				break;
			case SOU:
				_world[_ptX + _s * _ptY] |= SOU; _ptY++;
				_world[_ptX + _s * _ptY] = NOR | NOR << 4;
				break;
			case WES:
				_world[_ptX + _s * _ptY] |= WES; _ptX--;
				_world[_ptX + _s * _ptY] = EAS | EAS << 4;
			}
		}
	}

	int getDirection()
	{
		int d = 1 << rand() % 4;
		while (true)
		{
			for (int x = 0; x < 4; x++)
			{
				if (testDir(d)) return d;
				d <<= 1;
				if (d > 8) d = 1;
			}
			d = (_world[_ptX + _s * _ptY] & 0xf0) >> 4;
			if (!d) return -1;
			switch (d)
			{
			case NOR: _ptY--; break;
			case EAS: _ptX++; break;
			case SOU: _ptY++; break;
			case WES: _ptX--; break;
			}
			d = 1 << rand() % 4;
		}
	}

	bool testDir(int d)
	{
		switch (d)
		{
		case NOR: return (_ptY - 1 > -1 && !_world[_ptX + _s * (_ptY - 1)]);
		case EAS: return (_ptX + 1 < _s && !_world[_ptX + 1 + _s * _ptY]);
		case SOU: return (_ptY + 1 < _s && !_world[_ptX + _s * (_ptY + 1)]);
		case WES: return (_ptX - 1 > -1 && !_world[_ptX - 1 + _s * _ptY]);
		}
		return false;
	}

	void killArray() 
	{ 
		if (_world) delete[] _world; 
		if (m_map) delete[] m_map;
	}

#define N 5
#define mapY(y) (y * N + 1)
#define mapX(x) (x * N + 1)
#define UNBLOCK(x, y, adder) for (size_t i = 0; i < N - 1; i++) { m_map[x + y + adder] = 0;}

	void translateToMap()
	{
		auto size = _s * N + 1;
		m_map = new byte[size * size];
		memset(m_map, -1, size * size);

		for (int y = 0; y < _s; y++)
		{
			int yy = y * _s;

			int _yy = mapY(y) * size;
			int _yy_N = _yy - size;
			int _yy_S = _yy + size * (N - 1);
			int _yy_W = _yy;
			int _yy_E = _yy;

			for (int x = 0; x < _s; x++)
			{
				int _x = mapX(x);
				int _x_N = _x;
				int _x_S = _x;
				int _x_W = (_x - 1);
				int _x_E = (_x - 1) + N;

				BYTE b = _world[x + yy];

				for (size_t sy = 0; sy < N - 1; sy++)
				{
					auto syy = _yy + sy * size;
					for (size_t sx = 0; sx < N - 1; sx++)
					{
						m_map[_x + sx + syy] = 0;
					}
				}

				if ((b & NOR))
				{
					UNBLOCK(_x_N, _yy_N, i);
					//m_map[_x_N + _yy_N] = 0;
				}
				if ((b & EAS))
				{
					UNBLOCK(_x_E, _yy_E, i);
					//m_map[_x_E + _yy_E] = 0;
				}
				if ((b & SOU))
				{
					UNBLOCK(_x_S, _yy_S, i * size);
					//m_map[_x_S + _yy_S] = 0;
				}
				if ((b & WES))
				{
					UNBLOCK(_x_W, _yy_W, i * size);
					//m_map[_x_W + _yy_W] = 0;
				}
			}
		}

		/*for (size_t y = 0; y < size; y++)
		{
			auto yy = y * size;
			for (size_t x = 0; x < size; x++)
			{
				std::cout << ((bool)m_map[x + yy] ? '#' : ' ') << " ";
			}
			std::cout << "\n";
		}*/
	}

public:
	inline size_t Size()
	{
		return _s * N + 1;
	}

#undef N
#undef mapY
#undef mapX
#undef UNBLOCK

private:
	BYTE* _world;
	byte* m_map;
	int      _s, _ptX, _ptY;

};