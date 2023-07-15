#include "SlimeGenerator.h"

#include "Components2D/Rendering/AnimatedSpritesRenderer.h"
#include "Components2D/Physics/RigidBody2D.h"

#include "Monster.h"
#include "TAG.h"
#include "COLLISION_MASK.h"
#include "GameUtils.h"

#include "Stopwatch.h"

class SlimeScript : Traceable<SlimeScript>, public Monster
{
public:
	inline static ID IDLE_ANIMATION_ID				= INVALID_ID;
	inline static ID MOVE_LEFT_ANIMATION_ID			= INVALID_ID;
	inline static ID MOVE_RIGHT_ANIMATION_ID		= INVALID_ID;

protected:
	SCRIPT2D_DEFAULT_METHOD(SlimeScript);
	using Base = Monster;
	TRACEABLE_FRIEND();
	void Trace(Tracer* tracer)
	{
		Base::Trace(tracer);
	}

	AnimatedSpritesRenderer* m_renderer = nullptr;
	Renderer2D* m_hpBar = nullptr;
	Stopwatch m_hideHpBarStopwatch = {};
	ID m_curAnimId = INVALID_ID;

public:
	virtual void OnStart() override
	{
		m_renderer = GetObject()->GetComponentRaw<AnimatedSpritesRenderer>();
		m_hpBar = GetObject()->Child(0)->GetComponentRaw<Renderer2D>();
		m_hpBar->SetVisible(false);
	}

	virtual void OnIdle() override
	{
		if (m_curAnimId != IDLE_ANIMATION_ID)
		{
			m_renderer->SetAnimation(IDLE_ANIMATION_ID);
			m_curAnimId = IDLE_ANIMATION_ID;

			m_renderer->Play(Random::RangeInt32(0, 8) * (1.0f / 8.0f));
		}
	};

	virtual void OnUpdate(float dt) override
	{
		Base::OnUpdate(dt);
		m_hideHpBarStopwatch.Update(dt);

		if (m_hideHpBarStopwatch.IsTimeout())
		{
			m_hpBar->SetVisible(false);
		}
	}

	virtual void OnMove(const Vec2& dir) override
	{
		if (dir.x > 0)
		{
			if (m_curAnimId != MOVE_RIGHT_ANIMATION_ID)
			{
				m_renderer->SetAnimation(MOVE_RIGHT_ANIMATION_ID);
				m_curAnimId = MOVE_RIGHT_ANIMATION_ID;
			}
		}
		else
		{
			if (m_curAnimId != MOVE_LEFT_ANIMATION_ID)
			{
				m_renderer->SetAnimation(MOVE_LEFT_ANIMATION_ID);
				m_curAnimId = MOVE_LEFT_ANIMATION_ID;
			}
		}
	};

	virtual void OnTakeDamage(GameObject2D* from, float many) override
	{
		m_hpBar->SetVisible(true);

		m_hideHpBarStopwatch.Start(3.0f);
	}

};

SlimeGenerator::SlimeGenerator()
{
    m_collider = MakeShared<CircleCollider>(Vec2(0, 0), 25.0f);
}

Handle<GameObject2D> SlimeGenerator::New()
{
	auto monster = mheap::New<GameObject2D>(GameObject2D::DYNAMIC);
	auto renderer = monster->NewComponent<AnimatedSpritesRenderer>();
	auto physics = monster->NewComponent<RigidBody2D>(RigidBody2D::DYNAMIC, m_collider);
	auto script = monster->NewComponent<SlimeScript>();

	ID frameIds[8] = {};

	// move left animation
	for (size_t i = 0; i < 8; i++)
	{
		auto idx = i * 64;
		frameIds[i] = renderer->LoadSpriteFrame("monsters/Slime/slime.png", AARect({ idx, 0 }, { 64,64 }));
		auto& sprite = renderer->GetSpriteFrame(frameIds[i]);
		sprite.SetAnchorPoint({ 0.5f,0.6f });
		sprite.Transform().Scale() = { 1.1f,1.1f };
	}
	SlimeScript::MOVE_LEFT_ANIMATION_ID = renderer->MakeAnimation(frameIds, 8, 1.0f);

	// move right animation
	for (size_t i = 0; i < 8; i++)
	{
		auto idx = i * 64;
		frameIds[i] = renderer->LoadSpriteFrame("monsters/Slime/slime.png", AARect({ idx, 64 }, { 64,64 }));
		auto& sprite = renderer->GetSpriteFrame(frameIds[i]);
		sprite.SetAnchorPoint({ 0.5f,0.6f });
		sprite.Transform().Scale() = { 1.1f,1.1f };
	}
	SlimeScript::MOVE_RIGHT_ANIMATION_ID = renderer->MakeAnimation(frameIds, 8, 1.0f);

	// idle animation
	for (size_t i = 0; i < 8; i++)
	{
		auto idx = i * 64;
		frameIds[i] = renderer->LoadSpriteFrame("monsters/Slime/slime.png", AARect({ idx, 128 }, { 64,64 }));
		auto& sprite = renderer->GetSpriteFrame(frameIds[i]);
		sprite.SetAnchorPoint({ 0.5f,0.6f });
		sprite.Transform().Scale() = { 1.1f,1.1f };
	}
	SlimeScript::IDLE_ANIMATION_ID = renderer->MakeAnimation(frameIds, 8, 1.0f);

	renderer->ClearAABB();
	renderer->SetAnimation(SlimeScript::IDLE_ANIMATION_ID);

	physics->CollisionMask() = COLLISION_MASK::MONSTER;

	monster->Tag() = TAG::MONSTER;

	GameUtils::AddHpBar(monster, sf::Color::Red, 70, 100, &script->DynamicObjectProperties().hp)
		->Position() = { -35, -40 };

	return monster;
}
