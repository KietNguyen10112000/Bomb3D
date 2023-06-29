#pragma once

#include "Core/Memory/Memory.h"
#include "Core/Structures/String.h"

using namespace soft;

class PlayerScript;

class Item
{
public:
	virtual ~Item() {};

public:
	virtual bool Use(PlayerScript* player) = 0;
	inline virtual String GetDesc(PlayerScript* player) 
	{
		return "";
	};

};

class Coin : public Item
{
public:
	virtual bool Use(PlayerScript* player) override;

};