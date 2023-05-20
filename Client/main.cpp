#include "Plugins/Bridge/ScriptBridge.h"

#include "Engine/Engine.h"
#include "Engine/ENGINE_EVENT.h"

#include "PlayerScript.h"
#include "MapRenderer.h"

#include "Global.h"
#include "GameLoopHandler.h"

#include "GameActions/GameActions.h"

//std::array<std::array<size_t, 32>, 32> g_mapValues = {};

Global* Global::s_instance = {};

void Initialize(Engine* engine)
{
	GameActions::InitializeAllGameActions();

	Global::s_instance = new Global();

	TCP_SOCKET_DESCRIPTION desc;
	desc.host = "127.0.0.1";
	desc.port = 9023;
	desc.useNonBlocking = true;
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
	delete Global::Get().connector;
	delete Global::Get().gameLoop;
	delete Global::s_instance;

	std::cout << "Bye!\n";
}