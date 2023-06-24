#include "ItemIconLoader.h"

class CoinRendererLoader : public ItemIconLoader
{
	// Inherited via ItemRendererLoader
	virtual AnimatedSprites* New() override
	{
		constexpr static size_t NUM_FRAMES = 6;
		AnimatedSprites* sprite = new AnimatedSprites();

		ID ids[NUM_FRAMES] = {};
		for (size_t i = 0; i < NUM_FRAMES; i++)
		{
			ids[i] = sprite->LoadSpriteFrame(String::Format("items/0/{}.png", i));
			auto& frame = sprite->GetSpriteFrame(ids[i]);
			frame.SetSize({ 35,35 });
			frame.SetAnchorPoint({ 0.5f,0.5f });
		}

		sprite->SetAnimation(sprite->MakeAnimation(ids, NUM_FRAMES, 0.5f));
		
		return sprite;
	}
};


class PistolRendererLoader : public ItemIconLoader
{
	// Inherited via ItemRendererLoader
	virtual AnimatedSprites* New() override
	{
		constexpr static size_t NUM_FRAMES = 30;
		AnimatedSprites* sprite = new AnimatedSprites();
		ID ids[NUM_FRAMES] = {};
		for (size_t i = 0; i < NUM_FRAMES; i++)
		{
			ids[i] = sprite->LoadSpriteFrame(String::Format("items/1/pistol ({}).png", i + 1));
			auto& frame = sprite->GetSpriteFrame(ids[i]);
			frame.SetSize({ 40,40 });
			frame.SetAnchorPoint({ 0.5f,0.5f });
		}
		sprite->SetAnimation(sprite->MakeAnimation(ids, NUM_FRAMES, 2.1f));
		return sprite;
	}
};


class SmgRendererLoader : public ItemIconLoader
{
	// Inherited via ItemRendererLoader
	virtual AnimatedSprites* New() override
	{
		constexpr static size_t NUM_FRAMES = 30;
		AnimatedSprites* sprite = new AnimatedSprites();
		ID ids[NUM_FRAMES] = {};
		for (size_t i = 0; i < NUM_FRAMES; i++)
		{
			ids[i] = sprite->LoadSpriteFrame(String::Format("items/2/smg ({}).png", i + 1));
			auto& frame = sprite->GetSpriteFrame(ids[i]);
			frame.SetSize({ 100,100 });
			frame.SetAnchorPoint({ 0.5f,0.5f });
		}
		sprite->SetAnimation(sprite->MakeAnimation(ids, NUM_FRAMES, 2.1f));
		return sprite;
	}
};


ItemIconLoader* ItemIconLoader::s_loaders[256] = {};

void ItemIconLoader::Initialize()
{
	s_loaders[0] = new CoinRendererLoader();
	s_loaders[1] = new PistolRendererLoader();
	s_loaders[2] = new SmgRendererLoader();
}

void ItemIconLoader::Finalize()
{
	for (auto& v : s_loaders)
	{
		if (v)
		{
			delete v;
		}
	}
}

AnimatedSprites* ItemIconLoader::Load(ID id)
{
	if (!s_loaders[id])
	{
		return nullptr;
	}

	return s_loaders[id]->New();
}

void ItemIconLoader::Free(AnimatedSprites* s)
{
	delete s;
}
