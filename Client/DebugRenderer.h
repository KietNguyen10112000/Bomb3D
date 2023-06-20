#pragma once

#include "Components2D/Rendering/Renderer2D.h"
#include "Components2D/Rendering/Camera2D.h"

#include "UIDebugPathFinder.h"

using namespace soft;

class DebugRenderer : public Renderer2D, public UIDebugPathFinder
{
public:
	virtual void Render(RenderingSystem2D* rdr) override
	{
		UIDebugPathFinder::Render(rdr);
	}
};