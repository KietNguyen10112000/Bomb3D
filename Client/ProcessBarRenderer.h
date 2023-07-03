#pragma once

#include "Components2D/Rendering/Renderer2D.h"
#include "Components2D/Rendering/Camera2D.h"

using namespace soft;

class ProcessBarRenderer : public Renderer2D
{
public:
	sf::RectangleShape m_inner;
	//sf::RectangleShape m_outter;

	float m_width;
	float m_max;
	float* m_target;

	ProcessBarRenderer(const sf::Color& color, float width, float max, float* target) : m_width(width), m_max(max), m_target(target)
	{
		m_inner.setFillColor(color);
		m_inner.setOutlineColor(sf::Color::Black);
		m_inner.setOutlineThickness(2.5f);
		m_inner.setSize({ width, 10.0f });
		//m_outter.setSize({ width + 5.0f, 15.0f });
	}

	virtual AARect GetLocalAABB() override
	{
		return {};
	}

	virtual void Render(RenderingSystem2D* rdr) override
	{
		m_inner.setSize({ m_width * ((*m_target) / m_max), 10.0f });
		FlushTransform(m_inner);
		//FlushTransform(m_outter);
		//rdr->DrawSF(m_outter);
		rdr->DrawSF(m_inner);
	}

};