#include "VictoryTowerUI.h"

#include "Objects2D/GameObject2D.h"
#include "Components2D/Rendering/SpriteRenderer.h"

#include "ObjectGeneratorId.h"
#include "GameConfig.h"

#include "Global.h"
#include "PlayerScript.h"
#include "VictoryTowerScript.h"

String VictoryTowerUI::GetUIImagePath()
{
	return "buildings/victory_tower_0.png";
}

String VictoryTowerUI::GetUIName()
{
	return u8"Tháp chiến thắng";
}

float VictoryTowerUI::PrepareUI(soft::GameObject2D* object, PlayerScript* player)
{
	auto renderer = object->GetComponentRaw<SpriteRenderer>();
	renderer->Sprite().Initialize(GetUIImagePath(), AARect(), Transform2D());
	//renderer->Sprite().FitTextureSize({ 2 * GameConfig::CELL_SIZE, 2 * GameConfig::CELL_SIZE });
	renderer->ClearAABB();
	renderer->SetVisible(false);
	renderer->Sprite().SetAnchorPoint({ 0.5f,0.75f });

	return 100.0f;
}

bool VictoryTowerUI::IsAllowRotation()
{
    return false;
}

size_t VictoryTowerUI::GetBuildingObjectGeneratorId()
{
    return ObjectGeneratorId::VICTORY_TOWER;
}

void VictoryTowerUI::SetInfo(PlayerScript* player, soft::GameObject2D* building)
{
	auto script = building->GetComponentRaw<VictoryTowerScript>();
	script->m_team = &player->GetTeam();
	script->m_remainTime = 10;

	player->GetTeam().hasVictoryTower = true;
	//player->SetVictoryTower(script);

	if (player->GetTeamId() == Global::Get().GetMyTeamId())
	{
		return;
	}

	auto comp = building->GetComponentRaw<SpriteRenderer>();
	comp->ClearAABB();

	auto& sprite = comp->Sprite();
	sprite.Initialize("buildings/victory_tower_1.png", {}, {});
	sprite.SetAnchorPoint({ 0.5f,0.75f });
}

bool VictoryTowerUI::CheckCanBuild(const Transform2D& transform, PlayerScript* player)
{
	if (player->GetTeam().hasVictoryTower)
	{
		return false;
	}

	return Global::Get().gameMap.IsMovable(transform.GetTranslation());
}
