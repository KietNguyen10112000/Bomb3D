#include "SMG.h"

#include "BulletScript.h"
#include "PlayerScript.h"

Handle<GameObject2D> SMG::GetGunObject(PlayerScript* player)
{
	auto gunObj = mheap::New<GameObject2D>(GameObject2D::DYNAMIC);
	auto gunRdr = gunObj->NewComponent<SpriteRenderer>("smg2.png");
	gunRdr->Sprite().SetAnchorPoint(Vec2(100 / 331.0f, 40 / 120.0f));
	gunRdr->Sprite().Transform().Scale() = Vec2(0.3f);
	gunRdr->ClearAABB();
	gunObj->Position() = { 50 / 2.0f, 40 };
	return gunObj;
}

Handle<GameObject2D> SMG::NewBullet(PlayerScript* player, const Vec2& direction)
{
	auto bullet = mheap::New<GameObject2D>(GameObject2D::DYNAMIC);
	auto bulletRdr = bullet->NewComponent<SpriteRenderer>("red.png");
	bulletRdr->Sprite().FitTextureSize({ 60, 10 });
	bulletRdr->Sprite().SetAnchorPoint({ 0.5f, 0.5f });

	bullet->NewComponent<FastBulletScript>()->Setup(player->GetObject(), direction, 3000.0f, 10.0f);

	return bullet;
}

float SMG::GetRecoilTime(PlayerScript* player)
{
	return 0.1f;
}
