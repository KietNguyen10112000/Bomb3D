#pragma once

#include "ItemIconLoader.h"

class Preload
{
public:
	inline static void LoadAll()
	{
		ItemIconLoader::Initialize();
	}

	inline static void UnloadAll()
	{
		ItemIconLoader::Finalize();
	}

};