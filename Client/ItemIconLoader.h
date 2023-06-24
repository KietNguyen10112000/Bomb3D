#pragma once

#include "Objects2D/Rendering/AnimatedSprites.h"

using namespace soft;

class ItemIconLoader
{
public:
	static ItemIconLoader* s_loaders[256];

	static void Initialize();
	static void Finalize();

	static AnimatedSprites* Load(ID id);
	static void Free(AnimatedSprites* s);

	virtual ~ItemIconLoader() {};

protected:
	virtual AnimatedSprites* New() = 0;

};