#pragma once
#include "Core/Structures/String.h"
#include "Objects2D/GameObject2D.h"

using namespace soft;

class PlayerScript;

class Skill
{
public:
	enum SKILL_FLAG
	{
		NONE						= 0,
		FLAG_NO_MOVE				= (1 << 0),
	};

public:
	float m_coolDown = 0;

	virtual ~Skill() {};

public:
	// when player picked up this skill
	virtual void OnAcquired(PlayerScript* player, size_t skillId) = 0;

	// when player chose skill 
	virtual void OnChose(PlayerScript* player) = 0;

	// when player use skill, return true when skill activated successfully
	virtual bool Activate(PlayerScript* player) = 0;

	// return SKILL_FLAG
	virtual size_t Update(PlayerScript* player, float dt) = 0;

	// whenever skill ready to cooldown
	virtual bool IsReady() = 0;

	// return the ui when player hold skill to aim before using it
	virtual Handle<GameObject2D> GetPrepareSkillUI(PlayerScript* player) = 0;

	virtual float GetCooldownTime() = 0;

	// skill desc
	virtual String GetGUIImgPath() = 0;
	virtual String GetDesc() = 0;

};