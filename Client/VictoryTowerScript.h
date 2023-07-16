#pragma once
#include "BaseDynamicObjectScript.h"
#include "TeamInfo.h"
#include "Global.h"

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
	virtual void OnDestroyed(GameObject2D* by) override
	{
		if (Global::Get().isGameOver)
		{
			return;
		}

		m_team->isVictory = false;
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