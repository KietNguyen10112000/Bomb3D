#pragma once

#include "Components2D/Script/Script2D.h"

using namespace soft;

#include "Global.h"

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
	int m_collisionCount = 0;

	GameObject2D* m_wall = 0;

	Vec2 m_pos;

public:
	virtual void OnStart() override
	{
		m_renderer = GetObject()->GetComponent<SpritesRenderer>().Get();
		m_body = GetObject()->GetComponent<RigidBody2D>().Get();
	}

	virtual void OnUpdate(float dt) override
	{
		auto dir0 = Global::Get().gameMap.m_pathFinder.GetDir(Position());

		if (dir0 == Vec2::ZERO)
		{
			m_scene->RemoveObject(GetObject());
			return;
		}

		if (m_wall)
		{
			auto dw = (m_wall->Position() - Position());
			if (dw.x > GameConfig::CELL_SIZE + 5.0f || dw.y > GameConfig::CELL_SIZE + 5.0f)
			{
				m_wall = nullptr;
			}
		}

		if (m_pos == Position() && m_wall)
		{
			auto dir1 = Vec2(-dir0.y, dir0.x);
			auto centerToWall = (m_wall->Position() - Position()).Normalize();
			if (centerToWall.Dot(dir1) > 0)
			{
				dir1 = -dir1;
			}

			dir0 += dir1;
			dir0.Normalize();
		}

		Position() += dir0 * 200.0f * dt;

		m_pos = Position();

		assert(m_collisionCount >= 0);
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

		if (another->Type() == GameObject2D::STATIC)
		{
			m_wall = another;
		}

		//std::cout << "Enter " << m_collisionCount << "\n";
	}

	virtual void OnCollisionExit(GameObject2D* another, const Collision2DPair& pair) override
	{
		//m_renderer->SetSprite(1);
		m_collisionCount--;

		/*if (another == m_wall)
		{
			m_wall = nullptr;
		}*/

		//std::cout << "Exit " << m_collisionCount << "\n";
	}

};