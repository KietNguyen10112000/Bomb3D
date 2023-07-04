#pragma once

#include "Components2D/Script/Script2D.h"
#include "Components2D/Rendering/SpriteRenderer.h"
#include "Components2D/Rendering/Camera2D.h"
#include "Components2D/Rendering/SpritesRenderer.h"
#include "Components2D/Physics/Physics2D.h"
#include "Components2D/Physics/RigidBody2D.h"

#include "Objects2D/Physics/Colliders/AARectCollider.h"
#include "Objects2D/Physics/Colliders/RectCollider.h"

#include "TAG.h"
#include "PlayerScript.h"
#include "Monster.h"

using namespace soft;

class BulletScript : Traceable<BulletScript>, public Script2D
{
protected:
	SCRIPT2D_DEFAULT_METHOD(BulletScript);
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

class FastBulletScript : Traceable<FastBulletScript>, public Script2D
{
protected:
	SCRIPT2D_DEFAULT_METHOD(FastBulletScript);
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
	float					m_damage;

public:
	virtual void OnUpdate(float dt) override
	{
		auto rayBegin = Position();
		Position() += m_dir * m_speed * dt;
		auto rayEnd = Position();

		auto queryRet = Physics()->RayQuery(rayBegin, rayEnd,
			Ray2DQueryInfo::SORT_MASK::SORT_OBJECT);

		if (queryRet)
		{
			auto v = m_speed;
			for (auto& obj : queryRet->objectResult)
			{
				// monster
				if (obj.object->Tag() == TAG::MONSTER)
				{
					auto body = obj.object->GetComponentRaw<RigidBody2D>();
					if (std::abs(body->m_desc.dynamic.v.Length()) < 500.0f)
					{
						v = v * 0.5f;
						body->ApplyForce({}, m_dir * v);
					}

					obj.object->GetComponentRaw<Monster>()->TakeDamage(m_from, m_damage);
				}
			}
			
			auto& obj = queryRet->objectResult[0];
			if (obj.object->Tag() != TAG::PLAYER)
			{
				m_scene->RemoveObject(GetObject());
			}
			else
			{
				auto player = obj.object->GetComponentRaw<PlayerScript>();
				if (player->GetTeamId()
					!= m_from->GetComponentRaw<PlayerScript>()->GetTeamId())
				{
					player->TakeDamage(m_from, m_damage);
					m_scene->RemoveObject(GetObject());
				}
			}
		}
	}

	inline void Setup(const Handle<GameObject2D>& obj, const Vec2& dir, float speed, float damage)
	{
		m_from = obj;
		m_dir = dir;
		m_speed = speed;
		m_damage = damage;
	}
};