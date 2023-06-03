#pragma once

#include <atomic>

// just allow 1 cosumer, 1 producer
template <typename T, size_t N>
class FixedSizeConcurrentQueue
{
private:
	T m_buffer[N];

	size_t m_headIdx = 0;
	size_t m_tailIdx = 0;

#ifdef _DEBUG

#endif // DEBUG


public:
	inline bool Empty() const 
	{
		return m_tailIdx == m_headIdx;
	}

	inline void Clear()
	{
		m_headIdx = 0;
		m_tailIdx = 0;
	}

	inline void enqueue(const T& v)
	{
		auto idx = m_tailIdx % N;

		m_buffer[idx] = v;
		m_tailIdx++;

		// =)))
		//assert(m_tailIdx != m_headIdx);
	}

	inline bool try_dequeue(T& v)
	{
		if (m_headIdx == m_tailIdx)
		{
			return false;
		}

		auto idx = m_headIdx % N;
		
		//assert(m_headIdx != m_tailIdx);

		v = m_buffer[idx];
		m_headIdx++;
		return true;
	}

};