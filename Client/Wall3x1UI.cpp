#include "Wall3x1UI.h"

#include "Objects2D/GameObject2D.h"
#include "Components2D/Rendering/SpriteRenderer.h"

#include "ObjectGeneratorId.h"
#include "GameConfig.h"

#include "Global.h"
#include "PlayerScript.h"
#include "Wall3x1Script.h"

String Wall3x1UI::GetUIImagePath()
{
	return "buildings/wall3x1_icon.png";
}

String Wall3x1UI::GetUIName()
{
	return u8"Tường 3x1";
}

float Wall3x1UI::PrepareUI(soft::GameObject2D* object, PlayerScript* player)
{
	auto renderer = object->GetComponentRaw<SpriteRenderer>();
	renderer->Sprite().Initialize("buildings/wall3x1.png", AARect(), Transform2D());
	renderer->Sprite().FitTextureSize({ 3 * GameConfig::CELL_SIZE, GameConfig::CELL_SIZE });
	renderer->ClearAABB();
	renderer->SetVisible(false);
	renderer->Sprite().SetAnchorPoint({ 0.5f,0.5f });

	return 100.0f;
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
	auto script = building->GetComponentRaw<Wall3x1Script>();
	script->m_teamId = player->GetTeamId();
	script->m_color = sf::Color::White;

	if (Global::Get().GetMyTeamId() != player->GetTeamId())
	{
		script->m_color = { 255, 200, 200, 255 };
		building->GetComponentRaw<SpriteRenderer>()->Sprite().SetColor(255, 200, 200, 255);
	}
}

bool Wall3x1UI::CheckCanBuild(const Transform2D& transform, PlayerScript* player)
{
	return true;
}
