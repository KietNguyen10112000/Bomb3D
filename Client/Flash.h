#pragma once
#include "Skill.h"

class Flash : public Skill
{
public:
	// Inherited via Skill
	virtual void OnAcquired(PlayerScript* player) override;
	virtual void OnChose(PlayerScript* player) override;
	virtual void ShowGUI(PlayerScript* player) override;
	virtual void Activate(PlayerScript* player) override;
	virtual size_t Update(PlayerScript* player, float dt) override;

};