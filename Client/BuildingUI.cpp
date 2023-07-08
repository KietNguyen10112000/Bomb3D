#include "BuildingUI.h"

#include "VictoryTowerUI.h"
#include "Wall3x1UI.h"

BuildingUI* BuildingUI::s_instances[256] = {};

void BuildingUI::Initialize()
{
	s_instances[0] = new VictoryTowerUI();
	s_instances[1] = new Wall3x1UI();
}

void BuildingUI::Finalize()
{
	for (auto& gen : s_instances)
	{
		if (gen)
		{
			delete gen;
		}
	}
}
