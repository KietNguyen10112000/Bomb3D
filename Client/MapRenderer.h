#pragma once

#include "Components2D/Rendering/Renderer2D.h"
#include "Components2D/Rendering/Camera2D.h"

using namespace soft;

class MapRenderer : public Renderer2D
{
private:
	struct LoadedSprite
	{
		Resource<Texture2D> rc;
		sf::Sprite sprite;
	};

	Vec2 m_cellSize = {};
	std::vector<LoadedSprite> m_sprites;
	std::vector<size_t> m_map;
	size_t m_width;
	size_t m_height;

public:
	MapRenderer(size_t width, size_t height, const Vec2& cellSize, size_t cellValueMax)
		: m_width(width), m_height(height), m_cellSize(cellSize)
	{
		m_sprites.resize(cellValueMax + 1);
		m_map.resize(m_width * m_height);

		m_zOrder = -99999;
	}

	void SetCellValue(size_t x, size_t y, size_t value)
	{
		assert(value < m_sprites.size());
		m_map[y * m_width + x] = value;
	}

	void LoadCell(size_t value, String path, const AARect& textureRect)
	{
		auto& s = m_sprites[value];
		s.rc = resource::Load<Texture2D>(path);
		s.sprite.setTexture(s.rc->GetSFTexture());
		SetSpriteTextureRect(s.sprite, textureRect);
		s.sprite.setScale(reinterpret_cast<sf::Vector2f&>(GetScaleFitTo(s.sprite, m_cellSize)));
	}

	virtual void Render(RenderingSystem2D* rdr) override
	{
		Vec2 temp[4];
		auto cam = rdr->GetCurrentCamera();
		AARect view = cam->GetView();
		view.GetPoints(temp);

		auto beginX = (intmax_t)std::floor(temp[0].x / m_cellSize.x);
		auto endX = (intmax_t)std::ceil(temp[1].x / m_cellSize.x);

		auto beginY = (intmax_t)std::floor(temp[0].y / m_cellSize.y);
		auto endY = (intmax_t)std::ceil(temp[2].y / m_cellSize.y);

		beginX = clamp(beginX, (intmax_t)0, (intmax_t)m_width);
		endX = clamp(endX, (intmax_t)0, (intmax_t)m_width);

		beginY = clamp(beginY, (intmax_t)0, (intmax_t)m_height);
		endY = clamp(endY, (intmax_t)0, (intmax_t)m_height);

		for (size_t y = beginY; y < endY; y++)
		{
			for (size_t x = beginX; x < endX; x++)
			{
				auto v = m_map[y * m_width + x];
				RenderSprite(rdr, m_sprites[v].sprite, 0, { x * m_cellSize.x, y * m_cellSize.y });
			}
		}
	}

};