#pragma once
#include "BaseDynamicObjectScript.h"
#include "TeamInfo.h"
#include "Global.h"

#include "PlayerScript.h"

class VictoryTowerScript : Traceable<VictoryTowerScript>, public BaseDynamicObjectScript
{
protected:
	SCRIPT2D_DEFAULT_METHOD(VictoryTowerScript);
	using Base = BaseDynamicObjectScript;
	TRACEABLE_FRIEND();
	void Trace(Tracer* tracer)
	{
		Base::Trace(tracer);
	}

public:
	TeamInfo* m_team = nullptr;
	float m_remainTime = 0;

public:
	virtual bool CanTakeDamage(GameObject2D* from, float many) override
	{
		if (from->Tag() == TAG::PLAYER)
		{
			if (&from->GetComponentRaw<PlayerScript>()->GetTeam() == m_team)
			{
				return false;
			}
		}

		return true;
	}

	virtual void OnDestroyed(GameObject2D* by) override
	{
		if (Global::Get().isGameOver)
		{
			return;
		}

		m_team->isVictory = false;

		if (m_team == &Global::Get().GetMyTeam())
		{
			Global::Get().GetOppositeTeam().isVictory = true;
		}
		else
		{
			Global::Get().GetMyTeam().isVictory = true;
		}

		Global::Get().isGameOver = true;
	}

	virtual void OnUpdate(float dt) override
	{
		if (Global::Get().isGameOver)
		{
			return;
		}

		m_remainTime -= dt;
		
		if (m_remainTime <= 0)
		{
			m_remainTime = 0;
			m_team->isVictory = true;
			Global::Get().isGameOver = true;
		}

		m_team->remainVictoryTime = m_remainTime;
	}

};