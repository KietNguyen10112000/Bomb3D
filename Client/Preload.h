#pragma once

#include "ItemIconLoader.h"
#include "ObjectGenerator.h"
#include "BuildingUI.h"
#include "GameFont.h"

class Preload
{
public:
	inline static void LoadAll()
	{
		GameFont::Initialize();
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