#include "VictoryTowerUI.h"

#include "Objects2D/GameObject2D.h"
#include "Components2D/Rendering/SpriteRenderer.h"

#include "ObjectGeneratorId.h"
#include "GameConfig.h"

#include "Global.h"
#include "PlayerScript.h"

void VictoryTowerUI::PrepareUI(soft::GameObject2D* object)
{
	auto renderer = object->GetComponentRaw<SpriteRenderer>();
	renderer->Sprite().Initialize("buildings/victory_tower_0.png", AARect(), Transform2D());
	//renderer->Sprite().FitTextureSize({ 3 * GameConfig::CELL_SIZE, GameConfig::CELL_SIZE });
	renderer->ClearAABB();
	renderer->SetVisible(false);
	renderer->Sprite().SetAnchorPoint({ 0.5f,0.5f });
}

bool VictoryTowerUI::IsAllowRotation()
{
    return false;
}

size_t VictoryTowerUI::GetBuildingObjectGeneratorId()
{
    return size_t();
}

void VictoryTowerUI::SetInfo(PlayerScript* player, soft::GameObject2D* building)
{
}

bool VictoryTowerUI::IsAllowOnNonMovable()
{
    return false;
}
