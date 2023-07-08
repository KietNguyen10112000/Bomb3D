#pragma once
#include "ObjectGenerator.h"

class VictoryTowerGenerator : public ObjectGenerator
{
	// Inherited via ObjectGenerator
	virtual Handle<GameObject2D> New() override;
};

