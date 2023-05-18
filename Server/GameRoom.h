#pragma once

#include "Network/TCPAcceptor.h"
#include "Network/TCPConnector.h"

#include "Synch/Package.h"

struct Client
{
	TCPConnector			conn;
	PackageReceiver			receiver;
};

struct GameRoom
{
	constexpr static size_t MAX_CLIENTS				= 10;

	size_t					id						= INVALID_ID;
	size_t					iterationCount			= 0;

	Client					clients[MAX_CLIENTS]	= {};
	size_t					clientsCount			= 0;

	PackageSender			sender;
	ByteStream				sendStream;

	inline void FixedIteration(float dt)
	{
		iterationCount++;

		for (size_t i = 0; i < clientsCount; i++)
		{
			auto& client = clients[i];
			if (!client.conn.IsDisconnected())
			{
				auto ercode = client.receiver.RecvSynch(client.conn);
				if (ercode == 0)
				{
					// success

					auto stream = client.receiver.GetStream();
					std::cout << stream.Get<String>() << "\n";
				}
				else if (ercode == PackageReceiver::ERCODE::CONNECTION_ERROR)
				{
					client.conn.Disconnect();
				}
			}
		}
	}

	inline auto& GetClientConn(size_t id)
	{
		return clients[id].conn;
	}

	inline void InitializeClient(size_t id)
	{
		clients[id].receiver.Initialize();
	}
};
