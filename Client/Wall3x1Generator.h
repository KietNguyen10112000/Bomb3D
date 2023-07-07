#pragma once
#include "ObjectGenerator.h"

#include "Core/Memory/SmartPointers.h"

#include "Objects2D/Physics/Colliders/RectCollider.h"

using namespace soft;

class Wall3x1Generator : public ObjectGenerator
{
protected:
	SharedPtr<RectCollider> m_collider;

public:
	Wall3x1Generator();

	// Inherited via ObjectGenerator
	virtual Handle<GameObject2D> New() override;
};

