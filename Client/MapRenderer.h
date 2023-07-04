#pragma once

#include "Components2D/Rendering/Renderer2D.h"
#include "Components2D/Rendering/Camera2D.h"
#include "Components2D/Script/Script2D.h"

using namespace soft;

#include "ItemIconLoader.h"

class MapRenderer : public Renderer2D
{
private:
	constexpr static size_t NUM_ICON_LAYERS = 8;

	struct LoadedSprite
	{
		Resource<Texture2D> rc;
		sf::Sprite sprite;
	};

	Vec2 m_cellSize = {};
	std::vector<LoadedSprite> m_sprites;

	size_t m_playedCount = 0;
	AnimatedSprites* m_itemSprites[NUM_ICON_LAYERS][256] = {};

	int m_itemSpriteLayer[GameMap::MAX_SIZE * GameMap::MAX_SIZE] = {};

	size_t m_width;
	size_t m_height;

public:
	MapRenderer(size_t width, size_t height, const Vec2& cellSize, size_t cellValueMax)
		: m_width(width), m_height(height), m_cellSize(cellSize)
	{
		m_sprites.resize(cellValueMax + 1);
		//m_map.resize(m_width * m_height);

		m_zOrder = -99999;

		for (size_t i = 0; i < 256; i++)
		{
			for (size_t j = 0; j < NUM_ICON_LAYERS; j++)
			{
				m_itemSprites[j][i] = ItemIconLoader::Load(i);

				if (m_itemSprites[j][i])
					m_itemSprites[j][i]->Play(Random::RangeFloat(0.016f, 1.6f));
			}
		}

		auto h = m_height;
		auto w = m_width;
		for (size_t y = 0; y < h; y++)
		{
			auto yy = y * w;
			for (size_t x = 0; x < w; x++)
			{
				m_itemSpriteLayer[x + yy] = Random::RangeInt32(0, NUM_ICON_LAYERS - 1);
			}
		}
	}

	~MapRenderer()
	{
		for (size_t i = 0; i < 256; i++)
		{
			for (size_t j = 0; j < NUM_ICON_LAYERS; j++)
			{
				if (m_itemSprites[j][i]) ItemIconLoader::Free(m_itemSprites[j][i]);
			}
		}
	}

	/*void SetCellValue(size_t x, size_t y, size_t value)
	{
		assert(value < m_sprites.size());
		m_map[y * m_width + x] = value;
	}*/

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
		auto* cells = Global::Get().gameMap.m_cells;
		auto* movable = Global::Get().gameMap.m_movable;

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

		auto count = GetObject()->GetScene()->GetIterationCount();
		if (m_playedCount != count)
		{
			for (size_t i = 0; i < 256; i++)
			{
				for (size_t j = 0; j < NUM_ICON_LAYERS; j++)
				{
					if (m_itemSprites[j][i])
						m_itemSprites[j][i]->Play(0.016f);
				}
			}
			m_playedCount = count;
		}

		for (size_t y = beginY; y < endY; y++)
		{
			for (size_t x = beginX; x < endX; x++)
			{
				auto idx = y * m_width + x;
				auto& cell = cells[idx];
				Vec2 pos = { x * m_cellSize.x, y * m_cellSize.y };
				RenderSprite(rdr, m_sprites[cell.value].sprite, 0, pos);

				auto renderIcon = m_itemSprites[m_itemSpriteLayer[idx]][cell.itemId];
				if (cell.itemId != 255 && movable[idx] && renderIcon)
				{
					RenderSprite(rdr, renderIcon->GetCurrentSpriteFrame(),
						{ 1,1 }, 0.0f, pos + Vec2(GameConfig::CELL_SIZE / 2.0f));
				}
			}
		}
	}

};