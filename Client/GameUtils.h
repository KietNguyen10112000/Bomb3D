#pragma once

#include "ProcessBarRenderer.h"

namespace GameUtils
{

inline auto* AddHpBar(GameObject2D* obj, const sf::Color& color, float width, float max, float* target)
{
	auto hpBar = mheap::New<GameObject2D>(GameObject2D::DYNAMIC);
	hpBar->NewComponent<ProcessBarRenderer>(color, width, max, target);
	obj->AddChild(hpBar);
	return hpBar.Get();
}

}