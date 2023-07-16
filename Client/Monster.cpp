#include "Monster.h"

#include "TAG.h"
#include "PlayerScript.h"

void Monster::OnDestroyed(GameObject2D* by)
{
	if (by && by->Tag() == TAG::PLAYER)
	{
		by->GetComponentRaw<PlayerScript>()->GetTeam().exp += m_destroyExp;
	}
	OnMonsterDestroyed(by);
}