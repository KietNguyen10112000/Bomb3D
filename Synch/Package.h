#pragma once

#include "Core/TypeDef.h"
#include "Network/TCPConnector.h"

#include "ByteStream.h"

using namespace soft;

// TCP package encapsulation | len | len-bytes | len |

struct Package
{
	constexpr static size_t MAX_LEN = 1 * MB;

	/*uint32_t len;

	inline byte* Begin() const
	{
		return (byte*)this;
	}

	inline constexpr size_t HeaderSize() const
	{
		return sizeof(len);
	}

	inline bool IsValid() const
	{
		return *(decltype(len)*)(Begin() + HeaderSize() + len) == len;
	}*/
};

#define READ_BLOCK_ON_NON_BLOCKING(ret, conn, buf, bufSize) if (ret == SOCKET_ERCODE::WOULD_BLOCK)	\
{																						\
	while (ret == SOCKET_ERCODE::WOULD_BLOCK)											\
	{																					\
		ret = conn.Recv(buf, bufSize);													\
	}																					\
}

class PackageReceiver
{
public:
	enum ERCODE
	{
		PACKAGE_EMPTY					= 1,
		PACKAGE_OVERSIZE				,
		PACKAGE_INVALID_FORMAT			,
		CONNECTION_ERROR
	};

	uint32_t m_pkgSize = 0;
	std::Vector<byte> m_buffer;

	inline void Initialize()
	{
		m_buffer.resize(Package::MAX_LEN);
	}

	// function for non blocking socket
	// return 0 on success
	inline int RecvSynch(TCPConnector& conn)
	{
		auto ret = conn.Recv((byte*)&m_pkgSize, sizeof(m_pkgSize));
		if (ret == SOCKET_ERCODE::WOULD_BLOCK)
		{
			return PACKAGE_EMPTY;
		}

		if (m_pkgSize > Package::MAX_LEN)
		{
			return PACKAGE_OVERSIZE;
		}

		auto realPkgSize = m_pkgSize + sizeof(uint32_t);

		uint32_t len = 0;
		while (len != realPkgSize)
		{
			ret = conn.Recv(&m_buffer[len], realPkgSize - len);
			if (ret == SOCKET_ERCODE::WOULD_BLOCK)
			{
				continue;
			}

			if (ret < 0)
			{
				return CONNECTION_ERROR;
			}

			len += ret;
		}

		auto size = m_pkgSize;
		m_pkgSize = *(uint32_t*)&m_buffer[m_pkgSize];

		if (size != m_pkgSize)
		{
			return PACKAGE_INVALID_FORMAT;
		}

		return 0;
	}

	inline ByteStreamRead GetStream() const
	{
		ByteStreamRead read;
		read.Initialize((byte*)m_buffer.data(), m_pkgSize);
		return read;
	}
};

class PackageSender
{
public:
	enum ERCODE
	{
		PACKAGE_OVERSIZE			= 1,
		CONNECTION_ERROR
	};

	inline int SendSynch(ByteStream& stream, TCPConnector& conn)
	{
		auto size = stream.PackSize();
		if (size > Package::MAX_LEN)
		{
			return PACKAGE_OVERSIZE;
		}

		int len = (int)size;
		auto buf = stream.m_begin;
		
		int sendLen = 0;
		while (len != 0)
		{
			sendLen = conn.Send(buf, len);
			if (sendLen < 0)
			{
				return CONNECTION_ERROR;
			}

			len -= sendLen;
			buf += sendLen;
		}

		return 0;
	}

};