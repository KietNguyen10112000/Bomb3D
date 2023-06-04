#pragma once

#include "Global.h"

#include "imgui/imgui.h"

#include "Objects2D/Rendering/Sprite.h"

#include "Components2D/Rendering/Camera2D.h"

#include "GameConfig.h"

class UIDebugPathFinder
{
public:
	Sprite m_arrowSprite;
	size_t m_mapWidth;
	size_t m_mapHeight;

	UIDebugPathFinder()
	{
		m_arrowSprite.Initialize("ui/arrow_right.png", {}, {});
		m_arrowSprite.Transform().Scale() = Vec2(0.3f);
		m_arrowSprite.SetAnchorPoint({ 0, 0.5f });

		m_mapWidth = Global::Get().gameMap.m_width;
		m_mapHeight = Global::Get().gameMap.m_height;
	}

	inline void Render(RenderingSystem2D* rdr)
	{
		if (Global::Get().setting.isDebugPathfinder == false)
		{
			return;
		}

		auto prevCells = Global::Get().gameMap.m_pathFinder.m_prev;
		auto movable = Global::Get().gameMap.m_movable;

		Vec2 temp[4];
		auto cam = rdr->GetCurrentCamera();
		AARect view = cam->GetView();
		view.GetPoints(temp);

		auto beginX = (intmax_t)std::floor(temp[0].x / (float)GameConfig::CELL_SIZE);
		auto endX = (intmax_t)std::ceil(temp[1].x / (float)GameConfig::CELL_SIZE);

		auto beginY = (intmax_t)std::floor(temp[0].y / (float)GameConfig::CELL_SIZE);
		auto endY = (intmax_t)std::ceil(temp[2].y / (float)GameConfig::CELL_SIZE);

		beginX = clamp(beginX, (intmax_t)0, (intmax_t)m_mapWidth);
		endX = clamp(endX, (intmax_t)0, (intmax_t)m_mapWidth);

		beginY = clamp(beginY, (intmax_t)0, (intmax_t)m_mapHeight);
		endY = clamp(endY, (intmax_t)0, (intmax_t)m_mapHeight);

		for (size_t y = beginY; y < endY; y++)
		{
			auto yy = y * m_mapWidth;
			for (size_t x = beginX; x < endX; x++)
			{
				if (!movable[x + yy]) continue;

				auto& prevCell = prevCells[x + yy];
				Vec2 dir = Vec2((float)prevCell.x - (float)x, (float)prevCell.y - (float)y);
				dir.Normalize();
				//dir.Round();

				float rotation = ((dir.y > 0) ? 1 : -1) * std::acos(dir.x);
				m_arrowSprite.Transform().Rotation() = rotation;
				m_arrowSprite.Transform().Translation() = { 
					x * (float)GameConfig::CELL_SIZE + (float)GameConfig::CELL_SIZE / 2.0f, 
					y * (float)GameConfig::CELL_SIZE + (float)GameConfig::CELL_SIZE / 2.0f 
				};
				rdr->DrawSprite(m_arrowSprite);
			}
		}
	}

};