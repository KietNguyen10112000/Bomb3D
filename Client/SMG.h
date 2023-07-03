#pragma once

#include "Gun.h"

class SMG : public Gun
{
	// Inherited via Gun
	virtual Handle<GameObject2D> GetGunObject(PlayerScript* player) override;
	virtual Handle<GameObject2D> NewBullet(PlayerScript* player, const Vec2& direction) override;
	virtual float GetRecoilTime(PlayerScript* player) override;

};