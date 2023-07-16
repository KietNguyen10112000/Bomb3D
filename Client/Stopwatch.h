#pragma once

#include <utility>

class Stopwatch
{
private:
	float m_remain = 0.0f;

public:
	inline void Start(float start, bool reset = false)
	{
		if (!reset && m_remain != 0.0f)
		{
			return;
		}

		m_remain = start;
	}

	inline void Update(float dt)
	{
		if (m_remain == 0.0f)
		{
			return;
		}

		m_remain = std::max(0.0f, m_remain - dt);
	}

	inline bool IsTimeout() const
	{
		return m_remain == 0.0f;
	}

	inline float Remain() const
	{
		return m_remain;
	}

};