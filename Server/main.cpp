#include "Plugins/Bridge/ScriptBridge.h"

#include "Engine/Engine.h"
#include "Engine/ENGINE_EVENT.h"

#include "Network/TCPAcceptor.h"
#include "Network/TCPConnector.h"

class ServerLoopHandler;
ServerLoopHandler* g_handler = nullptr;

Handle<TCPAcceptor> g_acceptor = nullptr;
Handle<TCPConnector> g_clients[10];
size_t g_clientCount = 0;

class ServerLoopHandler : public IterationHandler
{
	byte m_buffer[2048];

	// Inherited via IterationHandler
	virtual float DoIteration(float sumDt, Scene2D* scene) override
	{
		auto ret = g_acceptor->Accept(*g_clients[g_clientCount++].Get());

		std::cout << "Client connected " << g_clientCount << "\n";

		auto len = g_clients[g_clientCount - 1]->Recv(m_buffer, 2048);
		m_buffer[len] = '\0';
		std::cout << "Client msg: " << (char*)m_buffer << '\n';

		Thread::Sleep(100);
		return 0;
	}
};

void Initialize(Engine* engine)
{
	g_handler = new ServerLoopHandler();

	TCP_SOCKET_DESCRIPTION desc;
	desc.host = "0.0.0.0";
	desc.port = 9023;

	g_acceptor = mheap::New<TCPAcceptor>(desc, 10);
	for (auto& c : g_clients)
	{
		c = mheap::New<TCPConnector>();
	}
	engine->SetIterationHandler(g_handler);
	std::cout << "Hello, Server!\n";
}

void Finalize(Engine* engine)
{
	delete g_handler;
	std::cout << "Bye!\n";
}