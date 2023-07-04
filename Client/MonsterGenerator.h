#pragma once

#include "Objects2D/GameObject2D.h"

using namespace soft;

class MonsterGenerator
{
private:
	static MonsterGenerator* s_generators[256];

public:
	static void Initialize();
	static void Finalize();

	static Handle<GameObject2D> NewMonster(byte monsterId)
	{
		auto& gen = s_generators[monsterId];
		if (!gen)
		{
			return nullptr;
		}
		return gen->New();
	}

public:
	virtual ~MonsterGenerator() {};

	virtual Handle<GameObject2D> New() = 0;

};