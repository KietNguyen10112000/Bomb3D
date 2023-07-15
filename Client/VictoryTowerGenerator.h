#pragma once
#include "ObjectGenerator.h"

#include "Core/Memory/SmartPointers.h"

#include "Objects2D/Physics/Colliders/CircleCollider.h"

using namespace soft;

class VictoryTowerGenerator : public ObjectGenerator
{
protected:
	SharedPtr<CircleCollider> m_collider;

public:
	VictoryTowerGenerator();

	// Inherited via ObjectGenerator
	virtual Handle<GameObject2D> New() override;
};

