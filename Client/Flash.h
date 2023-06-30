#pragma once
#include "Skill.h"
#include "GameConfig.h"

#include "Math/Math.h"

class Flash : public Skill
{
protected:
	constexpr static size_t NUM_TICKS				= 5;
	constexpr static float	DISTANCE				= GameConfig::CELL_SIZE * 2.0f;
	constexpr static float	DISTANCE_PER_TICK		= DISTANCE / NUM_TICKS;

	Vec2 m_dir = { 0,0 };
	size_t m_remainMovingTick = 0;

public:
	// Inherited via Skill
	virtual void OnAcquired(PlayerScript* player, size_t skillId) override;
	virtual void OnChose(PlayerScript* player) override;
	
	virtual bool Activate(PlayerScript* player) override;
	virtual size_t Update(PlayerScript* player, float dt) override;

	virtual Handle<GameObject2D> GetPrepareSkillUI(PlayerScript* player) override;

	virtual String GetGUIImgPath() override;
	virtual String GetDesc() override;

	virtual bool IsReady() override;

	virtual float GetCooldownTime() override;

};