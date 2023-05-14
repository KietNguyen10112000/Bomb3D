#pragma once

#include "Engine/Engine.h"
#include "Objects2D/Scene2D/Scene2D.h"

#include "Global.h"

using namespace soft;

class GameLoopHandler : public IterationHandler
{
	// Inherited via IterationHandler
	virtual float DoIteration(float sumDt, Scene2D* scene) override
	{
		auto fixedDt = Global::Get().fixedDt;

		while (sumDt > fixedDt)
		{
			scene->PrevIteration();
			scene->Iteration();
			sumDt -= fixedDt;
		}

		scene->PostIteration();

		return sumDt;
	}
};