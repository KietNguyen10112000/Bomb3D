#pragma once

#include "Core/TypeDef.h"
#include "Core/Thread/Thread.h"

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
		SUCCEEDED						= 0,
		CONNECTION_BUSY					= 1,
		PACKAGE_OVERSIZE				,
		PACKAGE_INVALID_FORMAT			,
		CONNECTION_ERROR				,
	};

	int m_pkgSize = -1;
	int m_recvedPkgSize = 0;
	TCPConnector* m_curConn = nullptr;
	std::Vector<byte> m_buffer;

	inline void Initialize()
	{
		m_buffer.resize(Package::MAX_LEN);
	}

	//// function for non blocking socket
	//// return 0 on success
	//inline int RecvSynch(TCPConnector& conn)
	//{
	//	auto ret = conn.Recv((byte*)&m_pkgSize, sizeof(m_pkgSize));
	//	if (ret == SOCKET_ERCODE::WOULD_BLOCK)
	//	{
	//		return PACKAGE_EMPTY;
	//	}
	//	else if (ret < 0)
	//	{
	//		return CONNECTION_ERROR;
	//	}

	//	if (m_pkgSize > Package::MAX_LEN)
	//	{
	//		return PACKAGE_OVERSIZE;
	//	}

	//	auto realPkgSize = m_pkgSize + sizeof(uint32_t);

	//	uint32_t len = 0;
	//	while (len != realPkgSize)
	//	{
	//		ret = conn.Recv(&m_buffer[len], realPkgSize - len);
	//		if (ret == SOCKET_ERCODE::WOULD_BLOCK)
	//		{
	//			continue;
	//		}

	//		if (ret < 0)
	//		{
	//			return CONNECTION_ERROR;
	//		}

	//		len += ret;
	//	}

	//	auto size = m_pkgSize;
	//	m_pkgSize = *(uint32_t*)&m_buffer[m_pkgSize];

	//	if (size != m_pkgSize)
	//	{
	//		return PACKAGE_INVALID_FORMAT;
	//	}

	//	return 0;
	//}

	inline void Reset()
	{
		m_curConn = nullptr;
		m_recvedPkgSize = m_pkgSize;
		m_pkgSize = -1;
	}

	inline int TryRecv(TCPConnector& conn)
	{
		assert(m_curConn == nullptr || m_curConn == &conn);
		if (m_pkgSize == -1)
		{
			auto ret = conn.Recv((byte*)&m_pkgSize, sizeof(m_pkgSize));
			if (ret == SOCKET_ERCODE::WOULD_BLOCK)
			{
				Reset();
				return CONNECTION_BUSY;
			}
			else if (ret < 0)
			{
				Reset();
				return CONNECTION_ERROR;
			}

			if (m_pkgSize > Package::MAX_LEN)
			{
				Reset();
				return PACKAGE_OVERSIZE;
			}

			m_recvedPkgSize = 0;
			m_curConn = &conn;
		}
		
		auto realPkgSize = m_pkgSize + sizeof(uint32_t);

		while (m_recvedPkgSize != realPkgSize)
		{
			auto ret = conn.Recv(&m_buffer[m_recvedPkgSize], realPkgSize - m_recvedPkgSize);
			if (ret == SOCKET_ERCODE::WOULD_BLOCK)
			{
				//std::cout << "PackageReceiver::CONNECTION_BUSY\n";
				return CONNECTION_BUSY;
			} 
			else if (ret < 0)
			{
				Reset();
				return CONNECTION_ERROR;
			}

			m_recvedPkgSize += ret;
		}

		auto lastRet = SUCCEEDED;

		auto size = m_pkgSize;
		m_pkgSize = *(uint32_t*)&m_buffer[m_pkgSize];

		if (size != m_pkgSize)
		{
			lastRet = PACKAGE_INVALID_FORMAT;
		}

		Reset();

		return lastRet;
	}

	inline int RecvSynch(TCPConnector& conn)
	{
		int ret;
		while (true)
		{
			ret = TryRecv(conn);

			if (ret == ERCODE::CONNECTION_BUSY)
			{
				continue;
			}

			break;
		}

		return ret;
	}

	inline ByteStreamRead GetStream() const
	{
		ByteStreamRead read;
		read.Initialize((byte*)m_buffer.data() /*+ sizeof(uint32_t)*/, m_recvedPkgSize);
		return read;
	}
};

class PackageSender
{
public:
	enum ERCODE
	{
		SUCCEEDED					= 0,
		PACKAGE_OVERSIZE			= 1,
		CONNECTION_BUSY,
		CONNECTION_ERROR
	};

	ByteStream* m_curStream = nullptr;
	TCPConnector* m_curConn = nullptr;

	int m_pkgSize = 0;
	int m_remainPkgSize = 0;

	inline void Pack(ByteStream& stream)
	{
		stream.PackSize();
	}

	/*inline int SendSynch(ByteStream& stream, TCPConnector& conn)
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
	}*/

	inline void Reset()
	{
		m_curStream = nullptr;
		m_curConn = nullptr;
	}

	inline int TrySend(ByteStream& stream, TCPConnector& conn)
	{
		assert(m_curStream == nullptr || m_curStream == &stream);
		assert(m_curConn == nullptr || m_curConn == &conn);

		if (m_curStream == nullptr)
		{
			m_curStream = &stream;
			m_curConn = &conn;
			m_remainPkgSize = (int)stream.PackSize();
			m_pkgSize = m_remainPkgSize;

			if (m_remainPkgSize > Package::MAX_LEN)
			{
				Reset();
				return PACKAGE_OVERSIZE;
			}
		}

		auto buf = stream.m_begin + (m_pkgSize - m_remainPkgSize);
		while (m_remainPkgSize != 0)
		{
			auto sendLen = conn.Send(buf, m_remainPkgSize);

			if (sendLen == SOCKET_ERCODE::WOULD_BLOCK)
			{
				return CONNECTION_BUSY;
			}
			else if (sendLen < 0)
			{
				Reset();
				return CONNECTION_ERROR;
			}

			buf += sendLen;
			m_remainPkgSize -= sendLen;

			assert(m_remainPkgSize >= 0);
		}

		Reset();
		return SUCCEEDED;
	}

	inline int SendSynch(ByteStream& stream, TCPConnector& conn)
	{
		int ret;
		while (true)
		{
			ret = TrySend(stream, conn);

			if (ret == ERCODE::CONNECTION_BUSY)
			{
				continue;
			}

			break;
		}

		return ret;
	}
};