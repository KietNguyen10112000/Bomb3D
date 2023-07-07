#pragma once

#include "Objects2D/GameObject2D.h"

using namespace soft;

class ObjectGenerator
{
private:
	static ObjectGenerator* s_generators[256];

public:
	static void Initialize();
	static void Finalize();

	static Handle<GameObject2D> NewObject(ID id)
	{
		auto& gen = s_generators[id];
		if (!gen)
		{
			return nullptr;
		}
		return gen->New();
	}

public:
	virtual ~ObjectGenerator() {};

	virtual Handle<GameObject2D> New() = 0;

};