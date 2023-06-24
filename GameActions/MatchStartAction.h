#pragma once

#include "Synch/Action.h"

#include "GameActionConfig.h"

#include "Math/Math.h"

class MatchStartAction : public Action
{
public:
	struct ClientMatchingInfo
	{
		String	ipAddr;
		ID		id;
		Vec2	pos;
	};

	byte m_map[512 * KB];
	byte m_mapItems[512 * KB];
	uint32_t m_width;
	uint32_t m_height;

	ClientMatchingInfo m_clientInfo[GameActionConfig::MAX_CLIENTS] = {};
	uint32_t m_numClient;

	byte m_blockCellValues[256];
	uint32_t m_numBlockCell;

	ID m_userID = INVALID_ID;
	ID m_roomID = INVALID_ID;

	size_t m_sendUserIDIdx = INVALID_ID;

	MatchStartAction();

	inline static Action* Initializer()
	{
		return new MatchStartAction();
	}

	inline static void Finalizer(Action* a)
	{
		delete ((MatchStartAction*)a);
	}

	// Inherited via Action
	virtual void Serialize(ByteStream& stream) override;
	virtual void Deserialize(ByteStreamRead& stream) override;
	virtual void Activate(Scene2D* scene) override;

	inline void SetBlockCellsValue(const byte* blockCells, uint32_t count)
	{
		memcpy(m_blockCellValues, blockCells, count);
		m_numBlockCell = count;
	}

	inline void SetMapValues(const byte* mapValues, uint32_t width, uint32_t height)
	{
		memcpy(m_map, mapValues, width * height);
		m_width = width;
		m_height = height;
	}

	inline void SetClientInfo(ID id, String	ipAddr)
	{
		m_clientInfo[id].id = id;
		m_clientInfo[id].ipAddr = ipAddr;
	}

	inline void Clear()
	{
		m_width = 0;
		m_height = 0;
		m_numClient = 0;
		m_numBlockCell = 0;
	}

	inline void SetUserId(ID id)
	{
		m_userID = id;
	}
};