#pragma once

#include "ItemIconLoader.h"
#include "ObjectGenerator.h"
#include "BuildingUI.h"

class Preload
{
public:
	inline static void LoadAll()
	{
		ItemIconLoader::Initialize();
		ObjectGenerator::Initialize();
		BuildingUI::Initialize();
	}

	inline static void UnloadAll()
	{
		BuildingUI::Finalize();
		ObjectGenerator::Finalize();
		ItemIconLoader::Finalize();
	}

};