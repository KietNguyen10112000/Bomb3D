#pragma once

#include "Components2D/Script/Script2D.h"
#include "Components2D/Rendering/SpriteRenderer.h"
#include "Components2D/Rendering/Camera2D.h"
#include "Components2D/Rendering/SpritesRenderer.h"
#include "Components2D/Physics/Physics2D.h"
#include "Components2D/Physics/RigidBody2D.h"

#include "Objects2D/Physics/Colliders/AARectCollider.h"
#include "Objects2D/Physics/Colliders/RectCollider.h"

using namespace soft;

class BulletScript : Traceable<BulletScript>, public Script2D
{
protected:
	using Base = Script2D;
	TRACEABLE_FRIEND();
	void Trace(Tracer* tracer)
	{
		Base::Trace(tracer);
		tracer->Trace(m_from);
	}

	Handle<GameObject2D>	m_from;
	Vec2					m_dir;
	float					m_speed;

public:
	virtual void OnUpdate(float dt) override
	{
		Position() += m_dir * m_speed * dt;
	}

	virtual void OnCollide(GameObject2D* obj, const Collision2DPair& pair) override
	{
		//m_scene->RemoveObject(GetObject());
		//std::cout << "Bullet removed\n";
		m_speed = 0;

		//if (obj->GetComponentRaw<Physics2D>()->CollisionMask() 
		//	== GetObject()->GetComponentRaw<Physics2D>()->CollisionMask())
		{
			m_scene->RemoveObject(GetObject());
		}
	}

	inline void Setup(const Handle<GameObject2D>& obj, const Vec2& dir, float speed)
	{
		m_from = obj;
		m_dir = dir;
		m_speed = speed;
	}
};