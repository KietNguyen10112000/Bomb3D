#include "Plugins/Bridge/ScriptBridge.h"

#include "Engine/Engine.h"
#include "Engine/ENGINE_EVENT.h"

#include "PlayerScript.h"
#include "MapRenderer.h"

#include "Global.h"
#include "GameLoopHandler.h"

#include "GameActions/GameActions.h"
#include "Preload.h"

//std::array<std::array<size_t, 32>, 32> g_mapValues = {};

Global* Global::s_instance = {};

void Initialize(Engine* engine)
{
	GameActions::InitializeAllGameActions();

	Global::s_instance = new Global();

	Preload::LoadAll();

	char host[32] = "127.0.0.1";

	TCP_SOCKET_DESCRIPTION desc;
	desc.host = host;
	desc.port = 9023;
	desc.useNonBlocking = true;

	if (FileUtils::IsExist("Resources/Config/Server.txt"))
	{
		byte* buffer;
		size_t size;
		FileUtils::ReadFile("Resources/Config/Server.txt", buffer, size);
		std::string_view str = { (char*)buffer, size };

		auto begin = str.find('=');
		if (begin != std::string_view::npos)
		{
			auto endIter = std::find_if(str.begin() + begin, str.end(), [](int c) { return std::isspace(c) || c == '\r' || c == '\t' || c == '\n'; });
			auto dis = std::distance(str.begin() + begin, endIter) - 1;
			std::string_view hostStr = str.substr(begin + 1, dis);

			::memcpy(host, hostStr.data(), hostStr.length());
		}

		FileUtils::FreeBuffer(buffer);
	}

	Global::Get().connector = new TCPConnector(desc);
	if (Global::Get().connector->Connect() < 0)
	{
		std::cout << "Unable to connect to server. Aborted.\n";
		Throw();
	}
	Global::Get().connector->SetBlockingMode(false);

	Global::Get().gameLoop = new GameLoopHandler();
	Global::Get().gameLoop->StartUp(engine);
	engine->SetIterationHandler(Global::Get().gameLoop);

	/*engine->AddListener(ENGINE_EVENT::SCENE_ON_SETUP,
		[](int argc, void** argv, void*)
		{
			AddStaticObjects((Scene2D*)argv[1]);
		},
		0
	);

	engine->AddListener(ENGINE_EVENT::SCENE_ON_START,
		[](int argc, void** argv, void*)
		{
			AddDynamicObjects((Scene2D*)argv[1]);
		},
		0
	);*/
}

void Finalize(Engine* engine)
{
	Preload::UnloadAll();

	delete Global::Get().connector;
	delete Global::Get().gameLoop;
	delete Global::s_instance;

	std::cout << "Bye!\n";
}