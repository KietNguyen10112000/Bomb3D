#pragma once

#include "Core/TypeDef.h"

#include "Core/Structures/String.h"
#include "Core/Structures/STD/STDContainers.h"

using namespace soft;

class ByteStreamRead
{
protected:
	friend class Package;
	friend class PackageReceiver;

	byte* m_begin = nullptr;
	byte* m_end = nullptr;
	byte* m_cur = nullptr;

	inline void Initialize(byte* begin, size_t len)
	{
		m_begin		= begin;
		m_end		= begin + len;
		m_cur		= begin;
	}

public:
#define BYTE_STREAM_ASSERT_NO_OVERFLOW(len) assert((m_cur + len <= m_end) && "ByteStream overflow");

	inline const char* GetString()
	{
#ifdef _DEBUG
		bool valid = false;
		auto it = m_cur;
		while (it != m_end)
		{
			if (*it == 0)
			{
				valid = true;
				break;
			}
			it++;
		}
		if (!valid)
		{
			assert(0 && "Invalid string");
		}
#endif // _DEBUG

		auto ret = (char*)m_cur;
		m_cur += strlen(ret) + 1;
		return ret;
	}

	template <typename T>
	inline auto Get()
	{
		static_assert(std::is_const_v<T> == false, "Can not get const value");

		if constexpr (
			std::is_same_v<T, String>
			|| std::is_same_v<T, char*>)
		{
			return GetString();
		}

		BYTE_STREAM_ASSERT_NO_OVERFLOW(sizeof(T));
		auto p = (T*)m_cur;
		m_cur += sizeof(T);
		return *p;
	}

#undef BYTE_STREAM_ASSERT_NO_OVERFLOW

	template <typename T>
	inline void Skip()
	{
		static_assert(std::is_const_v<T> == false, "Can not skip const value");

		if constexpr (
			std::is_same_v<T, String>
			|| std::is_same_v<T, char*>)
		{
			m_cur += strlen((char*)m_cur) + 1;
			return;
		}

		BYTE_STREAM_ASSERT_NO_OVERFLOW(sizeof(T));
		m_cur += sizeof(T);
		return;
	}

	inline bool IsEmpty()
	{
		assert(m_cur <= m_end);
		return m_cur == m_end;
	}

	inline size_t GetPayloadSize() const
	{
		return m_cur - m_begin - sizeof(uint32_t);
	}

	inline size_t GetSize() const
	{
		return m_cur - m_begin;
	}
};

class ByteStream : public ByteStreamRead
{
protected:
	friend class PackageSender;

	std::vector<byte>	m_buffer;

	inline void GrowthBy(size_t size)
	{
		if (m_buffer.size() >= size)
		{
			return;
		}

		auto newSize = m_buffer.size() + size;
		m_buffer.resize(newSize);

		m_cur = m_buffer.data() + (m_begin - m_cur);
		m_begin = m_buffer.data();
		m_end = m_begin + newSize;
	}

	inline void SetHeaderSize(uint32_t size)
	{
		*(uint32_t*)m_begin = size;
	}

	inline uint32_t PackSize()
	{
		auto size = m_cur - m_begin - sizeof(uint32_t);
		SetHeaderSize(size);
		Put<uint32_t>(size);
		return m_cur - m_begin;
	}

public:
	//ByteStream()
	//{
	//	GrowthBy(sizeof(uint32_t));

	//	// put placeholder for package size
	//	Put<uint32_t>(0);
	//}

	// must be called before put anything
	inline void Initialize(size_t initSize)
	{
		GrowthBy(initSize + sizeof(uint32_t));

		// put placeholder for package size
		Put<uint32_t>(0);
	}

	inline size_t PutString(size_t len, const char* str)
	{
		len++;
		GrowthBy(len);
		::memcpy(m_cur, str, len);
		m_cur[len] = 0;

		size_t ret = m_cur - m_begin;
		m_cur += len;
		return ret;
	}

	template <typename T>
	inline auto Put(const T& v)
	{
		static_assert((!std::is_pointer_v<T>) || std::is_trivial_v<T> 
			|| std::is_same_v<T, String> || std::is_same_v<T, char*>,
			"Can not put non-trivial type");

		if constexpr (std::is_same_v<T, String>)
		{
			return PutString(v.length(), v.c_str());
		}

		if constexpr (std::is_same_v<T, char*>)
		{
			return PutString(::strlen(v), v);
		}

		GrowthBy(sizeof(T));
		auto p = (T*)m_cur;
		auto ret = m_cur - m_begin;
		m_cur += sizeof(T);
		*p = v;
		return ret;
	}

	inline void Clean()
	{
		m_cur = m_begin;
		Put<uint32_t>(0);
	}

	template <typename T>
	inline void Set(size_t idx, const T& v)
	{
		auto p = (T*)&m_begin[idx];
		*p = v;
	}

	inline void Merge(ByteStream& another)
	{
		auto growthSize = another.m_cur - another.m_begin - sizeof(uint32_t);

		if (growthSize == 0) return;

		GrowthBy(growthSize);

		memcpy(m_cur, another.m_begin + sizeof(uint32_t), growthSize);
		m_cur += growthSize;
	}
};