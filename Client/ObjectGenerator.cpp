#include "ObjectGenerator.h"

#include "ObjectGeneratorId.h"

#include "SlimeGenerator.h"
#include "Wall3x1Generator.h"

ObjectGenerator* ObjectGenerator::s_generators[256] = {};

void ObjectGenerator::Initialize()
{
	s_generators[ObjectGeneratorId::SLIME]					= new SlimeGenerator();

	s_generators[ObjectGeneratorId::WALL3x1]				= new Wall3x1Generator();
}

void ObjectGenerator::Finalize()
{
	for (auto& gen : s_generators)
	{
		if (gen)
		{
			delete gen;
		}
	}
}
