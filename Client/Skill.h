#pragma once

class PlayerScript;

class Skill
{
public:
	enum SKILL_FLAG
	{
		FLAG_NO_MOVE				= (1 << 0),
	};

public:
	virtual ~Skill() {};

public:
	virtual void OnAcquired(PlayerScript* player) = 0;
	virtual void OnChose(PlayerScript* player) = 0;
	virtual void ShowGameGUI(PlayerScript* player) = 0;
	virtual void Activate(PlayerScript* player) = 0;

	// return SKILL_FLAG
	virtual size_t Update(PlayerScript* player, float dt) = 0;

	virtual String GetGUIImgPath() = 0;
	virtual String GetDesc() = 0;

};