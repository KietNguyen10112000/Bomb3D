#pragma once
#include "MonsterGenerator.h"

#include "Core/Memory/SmartPointers.h"

#include "Objects2D/Physics/Colliders/CircleCollider.h"

using namespace soft;

class SlimeGenerator : public MonsterGenerator
{
private:
	SharedPtr<CircleCollider> m_collider;

public:
	SlimeGenerator();

	// Inherited via MonsterGenerator
	virtual Handle<GameObject2D> New() override;

};

