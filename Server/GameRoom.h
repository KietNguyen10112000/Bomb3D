#pragma once

#include "Network/TCPAcceptor.h"
#include "Network/TCPConnector.h"

#include "Synch/Package.h"
#include "Synch/Action.h"

struct Client
{
	TCPConnector			conn;
	PackageReceiver			receiver;
};

struct GameRoom
{
	constexpr static size_t MAX_CLIENTS				= 10;

	constexpr static size_t NUM_TICKS_PER_SEND		= 4;

	size_t					id						= INVALID_ID;
	size_t					iterationCount			= 0;

	Client					clients[MAX_CLIENTS]	= {};
	size_t					clientsCount			= 0;

	PackageSender			sender;
	ByteStream				sendPkg;
	ByteStream				iterationStreams	[NUM_TICKS_PER_SEND];

	uint32_t				actionsCount		[NUM_TICKS_PER_SEND];
	size_t					actionsCountPkgIdx	[NUM_TICKS_PER_SEND];

	inline void ResetIterationStreams()
	{
		size_t i = 0;
		auto iter = iterationCount;
		for (auto& stream : iterationStreams)
		{
			stream.Clean();
			stream.Put<size_t>(iter++);
			actionsCountPkgIdx[i] = stream.Put<uint32_t>(0);
			actionsCount[i] = 0;
			i++;
		}
	}

	inline void SynchAllClients()
	{
		sendPkg.Clean();

		for (size_t i = 0; i < NUM_TICKS_PER_SEND; i++)
		{
			iterationStreams[i].Set<uint32_t>(actionsCountPkgIdx[i], actionsCount[i]);
			//std::cout << "Tick " << i << ", num actions: " << actionsCount[i] << '\n';
		}

		for (auto& stream : iterationStreams)
		{
			sendPkg.Merge(stream);
		}

		for (auto& client : clients)
		{
			if (!client.conn.IsDisconnected())
			{
				sender.SendSynch(sendPkg, client.conn);
			}
		}

		ResetIterationStreams();
	}

	// check if user send a valid action
	inline bool IsActionValid(ID userId, Action* action)
	{
		return true;
	}

	inline void ProcessClientStream(ByteStreamRead& stream)
	{
		size_t minIteration = 0;

		while (!stream.IsEmpty())
		{
			auto clientIteration = stream.Get<size_t>();
			auto actionCount = stream.Get<uint32_t>();

			if (minIteration == 0)
			{
				minIteration = clientIteration;
			}

#ifdef _DEBUG
			if (minIteration <= clientIteration)
			{
				int x = 3;
				assert(minIteration <= clientIteration);
			}

			if (clientIteration <= iterationCount)
			{
				int x = 3;
				assert(clientIteration <= iterationCount);
			}
#endif // _DEBUG

			

			auto serverActivateIteration = iterationCount; //+ (clientIteration - minIteration);

			auto streamIdx = 0;//serverActivateIteration % NUM_TICKS_PER_SEND;

			auto& sendStream = iterationStreams[streamIdx];
			uint32_t& validAction = actionsCount[streamIdx];

			//std::cout << "Recv " << actionCount << " actions\n";

			for (size_t i = 0; i < actionCount; i++)
			{
				ActionID aId = stream.Get<ActionID>();
				auto action = ActionCreator::New(aId);
				action->Deserialize(stream);

				if (IsActionValid(i, action))
				{
					sendStream.Put<ActionID>(action->GetActionID());
					action->Serialize(sendStream);
					validAction++;
				}

				ActionCreator::Delete(action);
			}
		}
	}

	inline void FixedIteration(float dt)
	{
		iterationCount++;

		if (iterationCount % NUM_TICKS_PER_SEND == 0)
		{
			SynchAllClients();
		}

		for (auto& client : clients)
		{
			if (!client.conn.IsDisconnected())
			{
				auto ercode = client.receiver.RecvSynch(client.conn);
				if (ercode == 0)
				{
					// success

					auto clientStream = client.receiver.GetStream();
					ProcessClientStream(clientStream);
				}
				else if (ercode == PackageReceiver::ERCODE::CONNECTION_ERROR)
				{
					client.conn.Disconnect();
				}
			}
		}
	}

	inline void StartUp()
	{
		sendPkg.Initialize(Package::MAX_LEN);
		for (auto& stream : iterationStreams)
		{
			stream.Initialize(64 * KB);
		}

		ResetIterationStreams();
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
