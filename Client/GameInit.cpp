#include "Engine/Engine.h"
#include "Engine/ENGINE_EVENT.h"

#include "PlayerScript.h"
#include "MapRenderer.h"

#include "GameInit.h"

using namespace soft;

void AddStaticObjects(Scene2D* scene, byte* mapValues, size_t width, size_t height,
	byte* blockCells, byte blockCellCount)
{
	bool isCellBlocked[256] = {};
	for (size_t i = 0; i < blockCellCount; i++)
	{
		isCellBlocked[blockCells[i]] = true;
	}

	auto cellCollider = MakeShared<AARectCollider>(AARect({ 0,0 }, { 60,60 }));
	for (size_t y = 0; y < height; y++)
	{
		auto row = &mapValues[y * width];
		for (size_t x = 0; x < width; x++)
		{
			//row[x] = 1;// Random::RangeInt64(1, 2);

			//auto v = Random::RangeInt64(1, 10);

			//if (y == 0 || y == height - 1 || x == 0 || x == width - 1 || v == 10)
			//{
			//	row[x] = 0;
			//	auto object = mheap::New<GameObject2D>(GameObject2D::STATIC);
			//	object->NewComponent<Physics2D>(cellCollider)
			//		->CollisionMask() = 1ull | (1ull << 2) | (1ull << 3);
			//	object->Position() = { x * 60, y * 60 };
			//	scene->AddObject(object);
			//}

			//if (v == 10 && !(y == 0 || y == height - 1 || x == 0 || x == width - 1))
			//{
			//	row[x] = 2;
			//}

			if (isCellBlocked[row[x]])
			{
				// add static object as blocked cell
				auto object = mheap::New<GameObject2D>(GameObject2D::STATIC);
				object->NewComponent<Physics2D>(cellCollider)
					->CollisionMask() = 1ull | (1ull << 2) | (1ull << 3);
				object->Position() = { x * 60, y * 60 };
				scene->AddObject(object);
			}
		}
	}
}

void AddPlayer(Scene2D* scene, ID id, const Vec2& pos, size_t width, size_t height)
{
	auto player = mheap::New<GameObject2D>(GameObject2D::DYNAMIC);
	player->Position() = pos;
	auto sprites = player->NewComponent<SpritesRenderer>();
	sprites->SetSprite(sprites->Load(String::Format("Player_{}.png", id), AARect(), Vec2(50, 50)));
	sprites->Load(String::Format("PlayerUP_{}.png", id), AARect(), Vec2(50, 50));
	sprites->Load(String::Format("PlayerDOWN_{}.png", id), AARect(), Vec2(50, 50));
	sprites->Load(String::Format("PlayerLEFT_{}.png", id), AARect(), Vec2(50, 50));
	sprites->Load(String::Format("PlayerRIGHT_{}.png", id), AARect(), Vec2(50, 50));
	player->NewComponent<PlayerScript>()->SetUserId(id);

	auto cellCollider = MakeShared<AARectCollider>(AARect({ 0,0 }, { 50,50 }), Vec2(5, 5));
	player->NewComponent<RigidBody2D>(RigidBody2D::KINEMATIC, cellCollider)
		->CollisionMask() = (1ull << 2);

	if (Global::Get().userId == id)
	{
		Vec2 camViewSize = { 960 * 1.2f, 720 * 1.2f };
		auto camObj = mheap::New<GameObject2D>(GameObject2D::DYNAMIC);
		auto cam = camObj->NewComponent<Camera2D>(AARect({ 0,0 }, camViewSize));
		cam->SetClamp(camViewSize / 2.0f, Vec2(width * 60) - camViewSize / 2.0f);
		player->AddChild(camObj);
	}

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

void AddMapRenderer(Scene2D* scene, const byte* mapValues, size_t width, size_t height)
{
	auto map = mheap::New<GameObject2D>(GameObject2D::GHOST);
	auto mapRenderer = map->NewComponent<MapRenderer>(width, height, Vec2(60, 60), 10);

	for (size_t y = 0; y < height; y++)
	{
		auto row = &mapValues[y * width];
		for (size_t x = 0; x < width; x++)
		{
			mapRenderer->LoadCell(row[x], String::Format("{}.png", (size_t)row[x]), {});
			mapRenderer->SetCellValue(x, y, row[x]);
		}
	}

	scene->AddObject(map);
}

//void AddDynamicObjects(Scene2D* scene)
//{
//	AddPlayer(scene);
//	AddMapRenderer(scene);
//}