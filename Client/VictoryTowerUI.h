#pragma once
#include "BuildingUI.h"

class VictoryTowerUI : public BuildingUI
{
	// Inherited via BuildingUI
	virtual String GetUIImagePath() override;
	virtual String GetUIName() override;
	virtual float PrepareUI(soft::GameObject2D* object, PlayerScript* player);
	virtual bool IsAllowRotation();
	virtual size_t GetBuildingObjectGeneratorId();
	virtual void SetInfo(PlayerScript* player, soft::GameObject2D* building);
	virtual bool CheckCanBuild(const Transform2D& transform, PlayerScript* player) override;
};

