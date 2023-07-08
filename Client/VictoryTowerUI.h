#pragma once
#include "BuildingUI.h"

class VictoryTowerUI : public BuildingUI
{
	// Inherited via BuildingUI
	virtual void PrepareUI(soft::GameObject2D* object);
	virtual bool IsAllowRotation();
	virtual size_t GetBuildingObjectGeneratorId();
	virtual void SetInfo(PlayerScript* player, soft::GameObject2D* building);
	virtual bool IsAllowOnNonMovable();
};

