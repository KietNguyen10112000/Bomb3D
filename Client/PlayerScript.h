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

#include "GameActions/UserInputAction.h"

#include "Global.h"
#include "GameLoopHandler.h"

#include "GameActions/GameActionConfig.h"

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
	UserInputAction m_inputAction;
	UserInput* m_input;
	ID m_userId = 0;

	float m_speed = 300;
	//float m_rotationSpeed = 100;
	float m_recoil = 0;
	float m_recoilMax = 0.1f;
	Vec2 m_gunRecoilBegin;
	Vec2 m_gunRecoilEnd;
	float m_gunRecoilLen = 15;
	bool m_enableMouse = false;
	bool m_hidedUI = false;

	float m_redLineLen = 0;
	Vec2 m_crossHairPos = {};

	float m_prevGunRotation = 0;
	float m_curGunRotation = 0;
	float m_gunRotationLerpTimeMax = 0;
	float m_gunRotationLerpTime = 0;
	float m_gunRotationSpeed = 6 * PI;

	inline void RecordInputAction()
	{
		if (Input()->IsKeyPressed(KEYBOARD::ESC))
		{
			m_enableMouse = !m_enableMouse;
			Input()->SetClampCursorInsideWindow(m_enableMouse);
		}

		m_input->SetKey('W', Input()->IsKeyDown('W'));
		m_input->SetKey('S', Input()->IsKeyDown('S'));
		m_input->SetKey('A', Input()->IsKeyDown('A'));
		m_input->SetKey('D', Input()->IsKeyDown('D'));
		m_input->SetKey(KEYBOARD::MOUSE_LEFT, Input()->IsKeyDown(KEYBOARD::MOUSE_LEFT));

		auto& cursorPos = Input()->GetCursor().position;
		auto center = m_cam->GetWorldPosition(Vec2(cursorPos.x, cursorPos.y),
			Input()->GetWindowWidth(), Input()->GetWindowHeight());
		auto& position = Position();
		Vec2 d = { center.x - position.x - 25,  center.y - position.y - 40 };
		auto len = d.Length();
		d.Normalize();

		m_redLineLen = len;

		auto rotation = (d.y / std::abs(d.y)) * std::acos(d.Dot(Vec2::X_AXIS));
		m_input->SetRotation(rotation);

		m_crossHairPos = center - position;
	}

	// hide red line, cross hair
	inline void HidePlayerUI()
	{
		if (m_hidedUI) return;

		m_hidedUI = true;
		m_redLine->GetComponentRaw<Renderer2D>()->SetVisible(false);
		m_crossHair->GetComponentRaw<Renderer2D>()->SetVisible(false);
	}

	inline void SmoothGunRotation(float dt)
	{
		auto gunRotation = m_input->m_synchRotation;
		m_gun->Rotation() = gunRotation;
		/*if (m_curGunRotation != gunRotation)
		{
			if (std::abs(gunRotation - m_gun->Rotation()) >= PI)
			{
				m_gun->Rotation() = m_gun->Rotation() + (gunRotation / std::abs(gunRotation)) * 2 * PI;
			}

			m_prevGunRotation = m_gun->Rotation();
			m_curGunRotation = gunRotation;

			m_gunRotationLerpTimeMax = std::abs((m_curGunRotation - m_prevGunRotation) / m_gunRotationSpeed);
			m_gunRotationLerpTime = m_gunRotationLerpTimeMax;
		}

		if (m_gunRotationLerpTime > 0)
		{
			m_gun->Rotation() = Lerp(m_prevGunRotation, m_curGunRotation,
				1 - (m_gunRotationLerpTime / m_gunRotationLerpTimeMax));
		}
		else
		{
			m_gun->Rotation() = m_curGunRotation;
		}

		m_gunRotationLerpTime = std::max(m_gunRotationLerpTime - dt, 0.0f);*/
	}

	inline void SetUserId(ID id)
	{
		m_userId = id;
	}

	virtual void OnStart() override
	{
		m_input = &Global::Get().gameLoop->m_userInput[m_userId];
		m_inputAction.SetUserId(m_userId, m_input);

		m_renderer = GetObject()->GetComponent<SpritesRenderer>();

		size_t childIdx = 0;
		if (Global::Get().userId == m_userId)
		{
			m_cam = GetObject()->Child(childIdx++)->GetComponent<Camera2D>();
		}

		m_redLine = GetObject()->Child(childIdx++);
		m_gun = GetObject()->Child(childIdx++);
		m_crossHair = GetObject()->Child(childIdx++);

		Input()->SetClampCursorInsideWindow(m_enableMouse);

		m_bulletCollider = MakeShared<RectCollider>(Rect(-30, -5, 60, 10));

		m_gunRecoilEnd = m_gun->Position();
	}

	virtual void OnUpdate(float dt) override
	{
		m_input->Roll();
		if (m_userId == Global::Get().userId)
			RecordInputAction();
		else
			HidePlayerUI();

		Vec2 motion = { 0,0 };
		m_renderer->SetSprite(0);

		if (m_input->IsKeyDown('W'))
		{
			motion.y -= 1;
			m_renderer->SetSprite(1);
		}

		if (m_input->IsKeyDown('S'))
		{
			motion.y += 1;
			m_renderer->SetSprite(2);
		}

		if (m_input->IsKeyDown('A'))
		{
			motion.x -= 1;
			m_renderer->SetSprite(3);
		}

		if (m_input->IsKeyDown('D'))
		{
			motion.x += 1;
			m_renderer->SetSprite(4);
		}

		if (motion != Vec2::ZERO)
		{
			Position() += motion.Normalize() * m_speed * dt;
		}

		{
			m_recoil = std::max(m_recoil - dt, 0.0f);

			if (m_recoil > 0)
			{
				m_gun->Position() = Lerp(m_gunRecoilBegin, m_gunRecoilEnd, (m_recoilMax - m_recoil) / m_recoilMax);
			}
			else
			{
				m_gun->Position() = m_gunRecoilEnd;
			}

			if (m_input->IsKeyDown(KEYBOARD::MOUSE_LEFT))
			{
				Shoot(m_input->m_synchRotation);
			}

			SmoothGunRotation(dt);

			m_redLine->Rotation() = m_input->m_curRotation;
			m_redLine->Scale().x = m_redLineLen;

			m_crossHair->Position() = m_crossHairPos;
		}

		if (m_userId == Global::Get().userId)
		{
			Global::Get().ExecuteAction(&m_inputAction);
		}
	}

	/*virtual void OnCollide(GameObject2D* obj, const Collision2DPair& pair) override
	{
		std::cout << "Collide " << m_count++ <<"\n";
	}*/

	inline void Shoot(float rotation)
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
			Vec2 dir = { std::cos(rotation), std::sin(rotation) };

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