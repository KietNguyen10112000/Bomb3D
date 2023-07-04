#pragma once

#include "ItemIconLoader.h"

#include "MonsterGenerator.h"

class Preload
{
public:
	inline static void LoadAll()
	{
		ItemIconLoader::Initialize();
		MonsterGenerator::Initialize();
	}

	inline static void UnloadAll()
	{
		MonsterGenerator::Finalize();
		ItemIconLoader::Finalize();
	}

};