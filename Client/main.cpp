#include "Plugins/Bridge/ScriptBridge.h"

#include "Engine/Engine.h"
#include "Engine/ENGINE_EVENT.h"

#include "PlayerScript.h"
#include "MapRenderer.h"

#include "Global.h"
#include "GameLoopHandler.h"

std::array<std::array<size_t, 32>, 32> g_mapValues = {};

Global Global::s_instance = {};

void AddStaticObjects(Scene2D* scene)
{
	auto cellCollider = MakeShared<AARectCollider>(AARect({ 0,0 }, { 60,60 }));
	for (size_t y = 0; y < g_mapValues.size(); y++)
	{
		auto& row = g_mapValues[y];
		for (size_t x = 0; x < row.size(); x++)
		{
			row[x] = 1;// Random::RangeInt64(1, 2);

			auto v = Random::RangeInt64(1, 10);

			if (y == 0 || y == g_mapValues.size() - 1 || x == 0 || x == row.size() - 1 || v == 10)
			{
				row[x] = 0;
				auto object = mheap::New<GameObject2D>(GameObject2D::STATIC);
				object->NewComponent<Physics2D>(cellCollider)
					->CollisionMask() = 1ull | (1ull << 2) | (1ull << 3);
				object->Position() = { x * 60, y * 60 };
				scene->AddObject(object);
			}

			if (v == 10 && !(y == 0 || y == g_mapValues.size() - 1 || x == 0 || x == row.size() - 1))
			{
				row[x] = 2;
			}
		}
	}
}

void AddPlayer(Scene2D* scene)
{
	auto player = mheap::New<GameObject2D>(GameObject2D::DYNAMIC);
	player->Position() = { 800 / 2, g_mapValues.size() * 60 - 100 };
	auto sprites = player->NewComponent<SpritesRenderer>();
	sprites->SetSprite(sprites->Load("Player.png", AARect(), Vec2(50, 50)));
	sprites->Load("PlayerUP.png", AARect(), Vec2(50, 50));
	sprites->Load("PlayerDOWN.png", AARect(), Vec2(50, 50));
	sprites->Load("PlayerLEFT.png", AARect(), Vec2(50, 50));
	sprites->Load("PlayerRIGHT.png", AARect(), Vec2(50, 50));
	player->NewComponent<PlayerScript>();

	auto cellCollider = MakeShared<AARectCollider>(AARect({ 0,0 }, { 50,50 }), Vec2(5, 5));
	player->NewComponent<RigidBody2D>(RigidBody2D::KINEMATIC, cellCollider)
		->CollisionMask() = (1ull << 2);

	Vec2 camViewSize = { 960 * 1.2f, 720 * 1.2f };
	auto camObj = mheap::New<GameObject2D>(GameObject2D::DYNAMIC);
	auto cam = camObj->NewComponent<Camera2D>(AARect({ 0,0 }, camViewSize));
	cam->SetClamp(camViewSize / 2.0f, Vec2(g_mapValues[0].size() * 60) - camViewSize / 2.0f);
	player->AddChild(camObj);

	// red line
	auto line = mheap::New<GameObject2D>(GameObject2D::DYNAMIC);
	auto lineRdr = line->NewComponent<SpriteRenderer>("red.png");
	lineRdr->Sprite().FitTextureSize(Vec2(1, 5));
	lineRdr->Sprite().SetOpacity(64);
	lineRdr->ClearAABB();
	line->Position() = { 50 / 2.0f, 40 };
	player->AddChild(line);

	// gun
	auto gunObj = mheap::New<GameObject2D>(GameObject2D::DYNAMIC);
	auto gunRdr = gunObj->NewComponent<SpriteRenderer>("smg2.png");
	gunRdr->Sprite().SetAnchorPoint(Vec2(100 / 331.0f, 40 / 120.0f));
	gunRdr->Sprite().Transform().Scale() = Vec2(0.3f);
	gunRdr->ClearAABB();
	gunObj->Position() = { 50 / 2.0f, 40 };
	player->AddChild(gunObj);

	// crosshair
	auto crossHair = mheap::New<GameObject2D>(GameObject2D::DYNAMIC);
	auto crossHairRdr = crossHair->NewComponent<SpriteRenderer>("CrosshairsRed.png");
	crossHairRdr->Sprite().FitTextureSize({ 80, 80 });
	crossHairRdr->Sprite().SetAnchorPoint({ 0.5f,0.5f });
	crossHairRdr->Sprite().SetOpacity(128);
	crossHairRdr->ClearAABB();
	player->AddChild(crossHair);

	scene->AddObject(player);
}

void AddMapRenderer(Scene2D* scene)
{
	auto map = mheap::New<GameObject2D>(GameObject2D::GHOST);
	auto mapRenderer = map->NewComponent<MapRenderer>(g_mapValues[0].size(), g_mapValues.size(), Vec2(60, 60), 10);

	for (size_t y = 0; y < g_mapValues.size(); y++)
	{
		auto& row = g_mapValues[y];
		for (size_t x = 0; x < row.size(); x++)
		{
			mapRenderer->LoadCell(row[x], String::Format("{}.png", row[x]), {});
			mapRenderer->SetCellValue(x, y, row[x]);
		}
	}

	scene->AddObject(map);
}

void AddDynamicObjects(Scene2D* scene)
{
	AddPlayer(scene);
	AddMapRenderer(scene);
}

void Initialize(Engine* engine)
{
	std::cout << "Hello, World!\n";

	TCP_SOCKET_DESCRIPTION desc;
	desc.host = "127.0.0.1";
	desc.port = 9023;
	Global::Get().connector = new TCPConnector(desc);
	Global::Get().connector->Connect();
	Global::Get().connector->Send((byte*)"Hello from client!", 18);

	Global::Get().gameLoop = new GameLoopHandler();
	engine->SetIterationHandler(Global::Get().gameLoop);

	engine->AddListener(ENGINE_EVENT::SCENE_ON_SETUP,
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
	);
}

void Finalize(Engine* engine)
{
	delete Global::Get().gameLoop;
	delete Global::Get().connector;
	std::cout << "Bye!\n";
}