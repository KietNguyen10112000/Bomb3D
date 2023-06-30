#include "Flash.h"

#include "PlayerScript.h"

class FlashUIScript : Traceable<FlashUIScript>, public Script2D
{
public:
	SCRIPT2D_DEFAULT_METHOD(PlayerScript);
	using Base = Script2D;
	TRACEABLE_FRIEND();
	void Trace(Tracer* tracer)
	{
		Base::Trace(tracer);
	}

	Renderer2D* m_renderer = nullptr;
	PlayerScript* m_player = nullptr;

	virtual void OnStart() override
	{
		m_renderer = GetObject()->GetComponentRaw<Renderer2D>();
	}

	virtual void OnUpdate(float dt) override
	{
		if (!m_renderer->IsVisible())
		{
			return;
		}

		GetObject()->Rotation() = m_player->GetInputRotation();
	}

};

void Flash::OnAcquired(PlayerScript* player, size_t skillId)
{
}

void Flash::OnChose(PlayerScript* player)
{
}

bool Flash::Activate(PlayerScript* player)
{
	m_dir = player->GetForwardDir();
	auto destPos = player->CenterPosition() + m_dir * DISTANCE;
	if (!Global::Get().gameMap.IsMovable(destPos))
	{
		return false;
	}

	m_remainMovingTick = NUM_TICKS;
	player->GetPhysics()->SetDisable(true);
	return true;
}

size_t Flash::Update(PlayerScript* player, float dt)
{
	if (m_remainMovingTick != 0)
	{
		m_remainMovingTick--;
		player->Position() += m_dir * DISTANCE_PER_TICK;

		if (m_remainMovingTick == 0)
		{
			player->GetPhysics()->SetDisable(false);
		}

		return SKILL_FLAG::FLAG_NO_MOVE;
	}

	return SKILL_FLAG::NONE;
}

Handle<GameObject2D> Flash::GetPrepareSkillUI(PlayerScript* player)
{
	auto object = mheap::New<GameObject2D>(GameObject2D::DYNAMIC);
	auto renderer = object->NewComponent<SpriteRenderer>("skills/Flash/gui.png");
	renderer->ClearAABB();
	renderer->Sprite().SetAnchorPoint({ -0.5, 0.5f });
	renderer->Sprite().FitTextureSize({ 80, 50 });

	object->NewComponent<FlashUIScript>()->m_player = player;
	object->Position() = { 25,25 };
	return object;
}

String Flash::GetGUIImgPath()
{
	return "skills/Flash/flash_icon.png";
}

String Flash::GetDesc()
{
	return u8"Dịch chuyển một đoạn ngắn";
}

bool Flash::IsReady()
{
	return m_remainMovingTick == 0;
}

float Flash::GetCooldownTime()
{
	return 3.0f;
}
