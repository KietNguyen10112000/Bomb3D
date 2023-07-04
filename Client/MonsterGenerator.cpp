#include "MonsterGenerator.h"

#include "SlimeGenerator.h"

MonsterGenerator* MonsterGenerator::s_generators[256] = {};

void MonsterGenerator::Initialize()
{
	s_generators[1] = new SlimeGenerator();
}

void MonsterGenerator::Finalize()
{
	for (auto& gen : s_generators)
	{
		if (gen)
		{
			delete gen;
		}
	}
}
