#pragma once

#include "Components2D/Script/Script2D.h"

using namespace soft;

class Monster : Traceable<Monster>, public Script2D
{
protected:
	SCRIPT2D_DEFAULT_METHOD(Monster);
	using Base = Script2D;
	TRACEABLE_FRIEND();
	void Trace(Tracer* tracer)
	{
		Base::Trace(tracer);
	}

	SpritesRenderer* m_renderer;
	RigidBody2D* m_body;
	size_t m_collisionCount = 0;

public:
	virtual void OnStart() override
	{
		m_renderer = GetObject()->GetComponent<SpritesRenderer>().Get();
		m_body = GetObject()->GetComponent<RigidBody2D>().Get();
	}

	virtual void OnUpdate(float dt) override
	{
		auto dir = Global::Get().gameMap.m_pathFinder.GetDir(Position());
		if (dir == Vec2::ZERO)
		{
			m_scene->RemoveObject(GetObject());
		}

		Position() += dir * 100.0f * dt;

		//std::cout << "pos: " << Position().x << ", " << Position().y << "\n";

		if (m_collisionCount)
		{
			m_renderer->SetSprite(0);
		}
		else
		{
			m_renderer->SetSprite(1);
		}
	}

	virtual void OnCollisionEnter(GameObject2D* another, const Collision2DPair& pair) override
	{
		//m_renderer->SetSprite(0);
		m_collisionCount++;
	}

	virtual void OnCollisionExit(GameObject2D* another, const Collision2DPair& pair) override
	{
		//m_renderer->SetSprite(1);
		m_collisionCount--;
	}

};