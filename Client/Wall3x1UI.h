#pragma once
#include "BuildingUI.h"
class Wall3x1UI : public BuildingUI
{
	// Inherited via BuildingUI
	virtual void PrepareUI(soft::GameObject2D* object);
	virtual bool IsAllowRotation();
	virtual size_t GetBuildingObjectGeneratorId();
	virtual void SetInfo(PlayerScript* player, GameObject2D* building) override;
	virtual bool IsAllowOnNonMovable();
};

