#include "MatchStartAction.h"

#include "GameActions.h"

#ifdef _CLIENT
#include "Client/GameLoopHandler.h"
#include "Client/Global.h"
#endif // _SERVER

#ifdef _SERVER
#include "Server/GameRoom.h"
#endif // _SERVER


MatchStartAction::MatchStartAction() : Action(GameActions::ACTION_ID::MATCH_START)
{
}

void MatchStartAction::Serialize(ByteStream& stream)
{
	stream.Put(m_width);
	stream.Put(m_height);
	stream.PutArray(m_map, m_width * m_height);
	stream.PutArray(m_blockCellValues, m_numBlockCell);
	
	stream.Put(m_numClient);
	for (size_t i = 0; i < m_numClient; i++)
	{
		auto& client = m_clientInfo[i];
		stream.Put<byte>((byte)client.id);
		stream.Put<String>(client.ipAddr);
		stream.Put<Vec2>(client.pos);
	}

	m_sendUserIDIdx = stream.Put(m_userID);
	stream.Put(m_roomID);
}

void MatchStartAction::Deserialize(ByteStreamRead& stream)
{
	m_width		= stream.Get<uint32_t>();
	m_height	= stream.Get<uint32_t>();

	uint32_t temp;
	stream.GetArray(m_map, sizeof(m_map), temp);
	stream.GetArray(m_blockCellValues, sizeof(m_blockCellValues), m_numBlockCell);

	m_numClient = stream.Get<uint32_t>();
	for (size_t i = 0; i < m_numClient; i++)
	{
		auto& client	= m_clientInfo[i];
		client.id		= stream.Get<byte>();
		client.ipAddr	= stream.Get<String>();
		client.pos		= stream.Get<Vec2>();
	}

	m_userID = stream.Get<ID>();
	m_roomID = stream.Get<ID>();
}

void MatchStartAction::Activate(Scene2D* scene)
{
	if (m_userID == INVALID_ID)
	{
		// client side activate
#ifdef _CLIENT
		Global::Get().gameLoop->OnRecvMatchStart(*this);
#endif
		return;
	}

#ifdef _SERVER
	// server side activate
	GameRoom::GetRoom(m_roomID)->OnClientMatchedSuccess(m_userID);
#endif // _SERVER
}
