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
	size_t m_collisionCount = 0;

public:
	virtual void OnStart() override
	{
		m_renderer = GetObject()->GetComponent<SpritesRenderer>().Get();
	}

	virtual void OnUpdate(float dt) override
	{
		auto& cursorPos = Input()->GetCursor().position;
		auto center = Global::Get().cam->GetWorldPosition(Vec2(cursorPos.x, cursorPos.y),
			Input()->GetWindowWidth(), Input()->GetWindowHeight());

		if (!Global::Get().setting.isOnConsole)
			Position() = center;

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