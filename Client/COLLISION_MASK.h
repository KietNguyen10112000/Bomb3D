#pragma once

class COLLISION_MASK
{
public:
	enum
	{
		PLAYER							= (1ull << 2),
		MONSTER							= (1ull << 2),
		WALL							= 1ull | PLAYER | (1ull << 3)
	};

};