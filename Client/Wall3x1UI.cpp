#include "Wall3x1UI.h"

#include "Objects2D/GameObject2D.h"
#include "Components2D/Rendering/SpriteRenderer.h"

#include "ObjectGeneratorId.h"
#include "GameConfig.h"

#include "Global.h"
#include "PlayerScript.h"

void Wall3x1UI::PrepareUI(soft::GameObject2D* object)
{
	auto renderer = object->GetComponentRaw<SpriteRenderer>();
	renderer->Sprite().Initialize("buildings/wall3x1.png", AARect(), Transform2D());
	renderer->Sprite().FitTextureSize({ 3 * GameConfig::CELL_SIZE, GameConfig::CELL_SIZE });
	renderer->ClearAABB();
	renderer->SetVisible(false);
	renderer->Sprite().SetAnchorPoint({ 0.5f,0.5f });
}

bool Wall3x1UI::IsAllowRotation()
{
    return true;
}

size_t Wall3x1UI::GetBuildingObjectGeneratorId()
{
    return ObjectGeneratorId::WALL3x1;
}

void Wall3x1UI::SetInfo(PlayerScript* player, GameObject2D* building)
{
	if (Global::Get().GetMyTeamId() != player->GetTeamId())
	{
		building->GetComponentRaw<SpriteRenderer>()->Sprite().SetColor(255, 200, 200, 255);
	}
}

bool Wall3x1UI::IsAllowOnNonMovable()
{
	return true;
}
