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

	inline String GetString()
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

		String ret = (char*)m_cur;
		m_cur += ret.length() + 1;
		return ret;
	}

	template <typename T>
	inline T Get()
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
	ByteStream(size_t initSize = 1 * MB)
	{
		GrowthBy(initSize + sizeof(uint32_t));

		// put placeholder for package size
		Put<uint32_t>(0);
	}

	inline void PutString(size_t len, const char* str)
	{
		len++;
		GrowthBy(len);
		::memcpy(m_cur, str, len);
		m_cur[len] = 0;
		m_cur += len;
	}

	template <typename T>
	inline void Put(const T& v)
	{
		static_assert((!std::is_pointer_v<T>) || std::is_trivial_v<T> 
			|| std::is_same_v<T, String> || std::is_same_v<T, char*>,
			"Can not put non-trivial type");

		if constexpr (std::is_same_v<T, String>)
		{
			PutString(v.length(), v.c_str());
			return;
		}

		if constexpr (std::is_same_v<T, char*>)
		{
			PutString(::strlen(v), v);
			return;
		}

		GrowthBy(sizeof(T));
		auto p = (T*)m_cur;
		m_cur += sizeof(T);
		*p = v;
	}

	inline void Clean()
	{
		m_cur = m_begin;
		Put<uint32_t>(0);
	}
};