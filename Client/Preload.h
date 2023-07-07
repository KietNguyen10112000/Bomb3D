#pragma once

#include "ItemIconLoader.h"

#include "ObjectGenerator.h"

class Preload
{
public:
	inline static void LoadAll()
	{
		ItemIconLoader::Initialize();
		ObjectGenerator::Initialize();
	}

	inline static void UnloadAll()
	{
		ObjectGenerator::Finalize();
		ItemIconLoader::Finalize();
	}

};