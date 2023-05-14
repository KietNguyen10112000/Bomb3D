#pragma once
#include "Components2D/Script/Script2D.h"
#include "Components2D/Rendering/SpriteRenderer.h"
#include "Components2D/Rendering/Camera2D.h"
#include "Components2D/Rendering/SpritesRenderer.h"
#include "Components2D/Physics/Physics2D.h"
#include "Components2D/Physics/RigidBody2D.h"

#include "Objects2D/Physics/Colliders/AARectCollider.h"
#include "Objects2D/Physics/Colliders/RectCollider.h"

#include "Input/Input.h"
#include "Input/KEYBOARD.h"

#include "BulletScript.h"

using namespace soft;

class PlayerScript : Traceable<PlayerScript>, public Script2D
{
protected:
	using Base = Script2D;
	TRACEABLE_FRIEND();
	void Trace(Tracer* tracer)
	{
		Base::Trace(tracer);
		tracer->Trace(m_renderer);
		tracer->Trace(m_cam);
		tracer->Trace(m_gun);
		tracer->Trace(m_redLine);
	}

	Handle<SpritesRenderer>			m_renderer;
	Handle<Camera2D>				m_cam;
	Handle<GameObject2D>			m_gun;
	Handle<GameObject2D>			m_redLine;
	Handle<GameObject2D>			m_crossHair;

	SharedPtr<RectCollider>			m_bulletCollider;

public:
	float m_speed = 300;
	float m_rotationSpeed = 100;
	float m_recoil = 0;
	float m_recoilMax = 0.1f;
	Vec2 m_gunRecoilBegin;
	Vec2 m_gunRecoilEnd;
	float m_gunRecoilLen = 15;
	bool m_enableMouse = false;

	virtual void OnStart() override
	{
		m_renderer = GetObject()->GetComponent<SpritesRenderer>();
		m_cam = GetObject()->Child(0)->GetComponent<Camera2D>();
		m_gun = GetObject()->Child(2);
		m_redLine = GetObject()->Child(1);
		m_crossHair = GetObject()->Child(3);

		Input()->SetClampCursorInsideWindow(m_enableMouse);

		m_bulletCollider = MakeShared<RectCollider>(Rect(-30, -5, 60, 10));

		m_gunRecoilEnd = m_gun->Position();
	}

	virtual void OnUpdate(float dt) override
	{
		Vec2 motion = { 0,0 };
		m_renderer->SetSprite(0);

		if (Input()->IsKeyDown('W'))
		{
			motion.y -= 1;
			m_renderer->SetSprite(1);
		}

		if (Input()->IsKeyDown('S'))
		{
			motion.y += 1;
			m_renderer->SetSprite(2);
		}

		if (Input()->IsKeyDown('A'))
		{
			motion.x -= 1;
			m_renderer->SetSprite(3);
		}

		if (Input()->IsKeyDown('D'))
		{
			motion.x += 1;
			m_renderer->SetSprite(4);
		}

		if (motion != Vec2::ZERO)
		{
			Position() += motion.Normalize() * m_speed * dt;
		}

		if (Input()->IsKeyPressed(KEYBOARD::ESC))
		{
			m_enableMouse = !m_enableMouse;
			Input()->SetClampCursorInsideWindow(m_enableMouse);
		}

		{
			auto& cursorPos = Input()->GetCursor().position;
			auto center = m_cam->GetWorldPosition(Vec2(cursorPos.x, cursorPos.y),
				Input()->GetWindowWidth(), Input()->GetWindowHeight());
			auto& position = Position();
			Vec2 d = { center.x - position.x - 25,  center.y - position.y - 40 };
			auto len = d.Length();
			d.Normalize();

			auto rotation = (d.y / std::abs(d.y)) * std::acos(d.Dot(Vec2::X_AXIS));

			m_recoil = std::max(m_recoil - dt, 0.0f);

			if (m_recoil > 0)
			{
				m_gun->Position() = Lerp(m_gunRecoilBegin, m_gunRecoilEnd, (m_recoilMax - m_recoil) / m_recoilMax);
			}
			else
			{
				m_gun->Position() = m_gunRecoilEnd;
			}

			if (Input()->IsKeyDown(KEYBOARD::MOUSE_LEFT))
			{
				Shoot(d, rotation);
			}

			m_gun->Rotation() = rotation;

			m_redLine->Rotation() = m_gun->Rotation();
			m_redLine->Scale().x = len;

			m_crossHair->Position() = center - position;
		}
	}

	/*virtual void OnCollide(GameObject2D* obj, const Collision2DPair& pair) override
	{
		std::cout << "Collide " << m_count++ <<"\n";
	}*/

	inline void Shoot(const Vec2& dir, float rotation)
	{
		//auto bullet = mheap::New<GameObject2D>(GameObject2D::DYNAMIC);
		//bullet->NewComponent<SpriteRenderer>("medium_bullet2.png")
		//	->Sprite().Transform().Scale() = { 0.5f,0.5f };
		//bullet->NewComponent<BulletScript>()->SetFrom(GetObject());
		//bullet->NewComponent<RigidBody2D>(RigidBody2D::KINEMATIC, m_bulletCollider);
		//bullet->Position() = Position();
		////bullet->Rotation() = PI / 2.0f;
		//m_scene->AddObject(bullet);

		if (m_recoil <= 0.0f)
		{
			auto bullet = mheap::New<GameObject2D>(GameObject2D::DYNAMIC);
			auto bulletRdr = bullet->NewComponent<SpriteRenderer>("red.png");
			bulletRdr->Sprite().FitTextureSize({ 60, 10 });
			bulletRdr->Sprite().SetAnchorPoint({ 0.5f, 0.5f });
			bullet->NewComponent<BulletScript>()->Setup(GetObject(), dir, 3000.0f);
			bullet->NewComponent<Physics2D>(Physics2D::SENSOR, m_bulletCollider)
				->CollisionMask() = (1ull << 3);
			bullet->Position() = Position();
			bullet->Position().x += 25;
			bullet->Position().y += 40;

			bullet->Position() += dir * 50;

			bullet->Rotation() = rotation;
			m_scene->AddObject(bullet);

			m_gunRecoilEnd = m_gun->Position();
			m_gunRecoilBegin = m_gun->Position() - dir * m_gunRecoilLen;
			m_gun->Position() = m_gunRecoilBegin;

			m_recoil = m_recoilMax;
		}
	}
};