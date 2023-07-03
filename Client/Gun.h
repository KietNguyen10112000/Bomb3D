#pragma once

#include "Objects2D/GameObject2D.h"

using namespace soft;

class PlayerScript;

class Gun
{
public:
	virtual ~Gun() {};

public:
	virtual Handle<GameObject2D> GetGunObject(PlayerScript* player) = 0;
	virtual Handle<GameObject2D> NewBullet(PlayerScript* player, const Vec2& direction) = 0;
	virtual float GetRecoilTime(PlayerScript* player) = 0;

};