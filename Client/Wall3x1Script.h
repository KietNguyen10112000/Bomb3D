#pragma once
#include "BaseDynamicObjectScript.h"
#include "TeamInfo.h"
#include "Global.h"

#include "PlayerScript.h"

class Wall3x1Script : Traceable<Wall3x1Script>, public BaseDynamicObjectScript
{
protected:
	SCRIPT2D_DEFAULT_METHOD(Wall3x1Script);
	using Base = BaseDynamicObjectScript;
	TRACEABLE_FRIEND();
	void Trace(Tracer* tracer)
	{
		Base::Trace(tracer);
	}

public:
	ID m_teamId = INVALID_ID;
	SpriteRenderer* m_renderer = nullptr;
	sf::Color m_color;

public:
	virtual bool CanTakeDamage(GameObject2D* from, float many) override
	{
		if (from->Tag() == TAG::PLAYER)
		{
			if (from->GetComponentRaw<PlayerScript>()->GetTeamId() == m_teamId)
			{
				return false;
			}
		}

		return true;
	}


	virtual void OnStart() override
	{
		m_renderer = GetObject()->GetComponentRaw<SpriteRenderer>();
	}

	virtual void OnUpdate(float dt) override
	{
		m_renderer->Sprite().SetColor(m_color.r, m_color.g, m_color.b, (byte)(255 * DynamicObjectProperties().hp / 100.0f));
	}

};
