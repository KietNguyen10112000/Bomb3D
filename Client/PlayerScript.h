#pragma once
#include "Components2D/Script/Script2D.h"
#include "Components2D/Rendering/SpriteRenderer.h"
#include "Components2D/Rendering/Camera2D.h"
#include "Components2D/Rendering/SpritesRenderer.h"
#include "Components2D/Physics/Physics2D.h"
#include "Components2D/Physics/RigidBody2D.h"

//#include "Objects2D/Physics/Colliders/AARectCollider.h"
//#include "Objects2D/Physics/Colliders/RectCollider.h"
#include "Objects2D/Physics/Colliders/CircleCollider.h"

#include "Input/Input.h"
#include "Input/KEYBOARD.h"

//#include "BulletScript.h"

#include "GameActions/UserInputAction.h"

#include "Global.h"
#include "GameLoopHandler.h"

#include "GameActions/GameActionConfig.h"

#include "Item.h"
#include "Skill.h"
#include "Flash.h"
#include "SMG.h"
#include "BaseDynamicObjectScript.h"
#include "TAG.h"
#include "Monster.h"

#include "ObjectGeneratorId.h"
#include "ObjectGenerator.h"

#include "BuildingUI.h"

using namespace soft;

class PlayerScript : Traceable<PlayerScript>, public BaseDynamicObjectScript
{
public:
	constexpr static size_t TICKS_TO_UPDATE_PATHFINDER = 15;

	struct PlayerData
	{
		size_t coin = 0;
	};

protected:
	SCRIPT2D_DEFAULT_METHOD(PlayerScript);
	using Base = BaseDynamicObjectScript;
	TRACEABLE_FRIEND();
	void Trace(Tracer* tracer)
	{
		Base::Trace(tracer);
		tracer->Trace(m_renderer);
		tracer->Trace(m_cam);
		tracer->Trace(m_gun);
		tracer->Trace(m_redLine);
		tracer->Trace(m_skills);
		tracer->Trace(m_skillsUI);
		tracer->Trace(m_skillsUIObject);
		tracer->Trace(m_guns);
	}

	Handle<SpritesRenderer>			m_renderer;
	Handle<Camera2D>				m_cam;
	Handle<GameObject2D>			m_gun;
	Handle<GameObject2D>			m_redLine;
	Handle<GameObject2D>			m_crossHair;

	//SharedPtr<RectCollider>			m_bulletCollider;

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

	PlayerData m_data;

	Handle<Skill> m_skills[5];
	Handle<Renderer2D> m_skillsUI[5];
	Handle<GameObject2D> m_skillsUIObject[5];
	size_t m_curSkillIdx = INVALID_ID;

	Handle<Gun> m_guns[2];
	size_t m_curGunId = 0;

	PathFinder* m_pathFinder = nullptr;
	Vec2 m_lastPosUpdatePathFinder = { 0,0 };
	size_t m_updatePathFinderCount = 0;
	CircleCollider m_circleCollider = { Vec2(0,0), 3.0f * GameConfig::CELL_SIZE };

	//SpriteRenderer* m_buildingUI = nullptr;
	GameObject2D* m_buildingUIObject = nullptr;

	BuildingUI* m_buildingUI = nullptr;
	ID m_curBuildingUiId = INVALID_ID;
	float m_curBuildingPrepareDist = 100.0f;

	inline void RecordInputAction()
	{
		if (Global::Get().setting.playerControlMode != GameSetting::PlayerControlMode::NONE)
		{
			return;
		}

		if (Input()->IsKeyPressed(KEYBOARD::ESC))
		{
			m_enableMouse = !m_enableMouse;
			Input()->SetClampCursorInsideWindow(m_enableMouse);
		}

		m_input->SetKey('W', Input()->IsKeyDown('W'));
		m_input->SetKey('S', Input()->IsKeyDown('S'));
		m_input->SetKey('A', Input()->IsKeyDown('A'));
		m_input->SetKey('D', Input()->IsKeyDown('D'));

		if (!Global::Get().setting.isStopPlayerLeftMouse)
		{
			m_input->SetKey(KEYBOARD::MOUSE_LEFT, Input()->IsKeyDown(KEYBOARD::MOUSE_LEFT));
		}
		else
		{
			m_input->SetKey(KEYBOARD::MOUSE_LEFT, false);
		}

		m_input->SetKey(KEYBOARD::MOUSE_RIGHT, Input()->IsKeyDown(KEYBOARD::MOUSE_RIGHT));
		m_input->SetKey(KEYBOARD::SPACE, Input()->IsKeyDown(KEYBOARD::SPACE));

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

	inline void CheckPickItem() 
	{
		auto center = Position() + Vec2(25, 25);
		auto& map = Global::Get().gameMap;
		auto item = map.GetItem(center);
		if (item != nullptr)
		{
			if (item->Use(this))
			{
				map.ClearItem(center);
			}
		}
	}

	inline void InitTestSkill()
	{
		auto flash = mheap::New<Flash>();
		auto flashUI = flash->GetPrepareSkillUI(this);
		flash->OnAcquired(this, 0);
		flash->OnChose(this);
		GetObject()->AddChild(flashUI);

		m_curSkillIdx = 0;
		m_skills[0] = flash;
		m_skillsUI[0] = flashUI->GetComponent<Renderer2D>();
		m_skillsUIObject[0] = flashUI;
	}

	inline void InitTestGun()
	{
		m_guns[0] = mheap::New<SMG>();
		m_curGunId = 0;
	}

	inline void InitBuildingUI()
	{
		auto buildingUI = mheap::New<GameObject2D>(GameObject2D::DYNAMIC);
		auto renderer = buildingUI->NewComponent<SpriteRenderer>("buildings/wall3x1.png", AARect(), Transform2D());
		renderer->ClearAABB();
		renderer->SetVisible(false);

		m_buildingUIObject = buildingUI;
		
		GetObject()->AddChild(buildingUI);

		SetBuildingSynch(1);
	}
	
	inline void SetBuilding(ID id)
	{
		m_buildingUI = BuildingUI::Get(id);
		if (m_buildingUI)
		{
			m_curBuildingUiId = id;

			m_curBuildingPrepareDist = m_buildingUI->PrepareUI(m_buildingUIObject, this);
		}
		else
		{
			m_curBuildingUiId = INVALID_ID;
		}
	}

	inline void SetBuildingSynch(ID id)
	{
		m_input->m_chooseBuildingId = id;
	}

	virtual void OnStart() override
	{
		m_input = &Global::Get().gameLoop->m_userInput[m_userId];
		m_inputAction.SetUserId(m_userId, m_input);

		InitTestSkill();
		InitTestGun();
		InitBuildingUI();

		m_gun = m_guns[m_curGunId]->GetGunObject(this);
		GetObject()->AddChild(m_gun);

		m_renderer = GetObject()->GetComponent<SpritesRenderer>();

		size_t childIdx = 0;
		if (Global::Get().userId == m_userId)
		{
			m_cam = GetObject()->Child(childIdx++)->GetComponent<Camera2D>();
			Global::Get().cam = m_cam.Get();
		}

		m_redLine = GetObject()->Child(childIdx++);
		//m_gun = GetObject()->Child(childIdx++);
		m_crossHair = GetObject()->Child(childIdx++);

		Input()->SetClampCursorInsideWindow(m_enableMouse);

		//m_bulletCollider = MakeShared<RectCollider>(Rect(-30, -5, 60, 10));

		m_gunRecoilEnd = m_gun->Position();

		m_pathFinder = Global::Get().gameMap.GetPlayerPathFinder(m_userId);
	}

	inline void MovePlayer(float dt)
	{
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
	}

	inline void UseSkill(float dt)
	{
		if (m_curSkillIdx == INVALID_ID)
		{
			return;
		}

		auto& curSkill = GetCurSkill();
		auto& curSkillUI = GetCurSkillUI();
		curSkillUI->SetVisible(false);
		if (curSkill->m_coolDown == 0 && m_input->IsKeyDown(KEYBOARD::MOUSE_RIGHT))
		{
			if (Global::Get().GetMyPlayer() == this)
			{
				curSkillUI->SetVisible(true);
			}
		}

		if (curSkill->m_coolDown != 0 && curSkill->IsReady())
		{
			if ((curSkill->m_coolDown -= dt) <= 0)
			{
				curSkill->m_coolDown = 0;
			}
		}

		//std::cout << m_input->m_prevSynchKey[KEYBOARD::MOUSE_RIGHT] << ", " << m_input->m_synchKey[KEYBOARD::MOUSE_RIGHT] << '\n';
		if (m_input->IsKeyUp(KEYBOARD::MOUSE_RIGHT) && curSkill->m_coolDown == 0)
		{
			if (curSkill->Activate(this))
			{
				curSkill->m_coolDown = curSkill->GetCooldownTime();
			}
		}
	}

	inline void UpdatePathFinder()
	{
		auto center = CenterPosition();
		if (m_lastPosUpdatePathFinder != center && (m_updatePathFinderCount++) % TICKS_TO_UPDATE_PATHFINDER == 0)
		{
			auto& pathFinder = m_pathFinder;
			pathFinder->Find(GetScene()->GetIterationCount(), pathFinder->GetCell(center));
			m_lastPosUpdatePathFinder = center;
		}
	}

	inline void ActivateNearMonsters()
	{
		auto info = Physics()->ColliderQuery(&m_circleCollider, GetObject()->GlobalTransform());
		if (info)
		{
			for (auto& object : info->Results())
			{
				if (object->Tag() == TAG::MONSTER)
				{
					object->GetComponentRaw<Monster>()->SetTarget(m_pathFinder);
				}
			}
		}
	}

	inline void CheckBuildObjects(float dt)
	{
		if (!m_buildingUI)
		{
			return;
		}

		// show UI
		if (m_input->IsKeyDown(KEYBOARD::SPACE))
		{
			if (Global::Get().GetMyPlayer() == this)
			{
				m_buildingUIObject->GetComponentRaw<Renderer2D>()->SetVisible(true);
			}

			auto pos = Vec2(m_curBuildingPrepareDist, 0);
			pos = (Vec3(pos, 1.0f) * Mat3::Rotation(m_input->m_synchRotation)).xy();
			m_buildingUIObject->Transform().Translation() = pos + Vec2(25, 25);

			if (m_buildingUI->IsAllowRotation())
			{
				m_buildingUIObject->Transform().Rotation() = m_input->m_synchRotation + PI / 2.0f;
			}
			else
			{
				m_buildingUIObject->Transform().Rotation() = 0;
			}
		}

		if (m_input->IsKeyUp(KEYBOARD::SPACE))
		{
			m_buildingUIObject->GetComponentRaw<Renderer2D>()->SetVisible(false);

			if (m_buildingUI->CheckCanBuild(m_buildingUIObject->GlobalTransform(), this))
			{
				auto object = ObjectGenerator::NewObject(m_buildingUI->GetBuildingObjectGeneratorId());
				object->Transform().Translation() = m_buildingUIObject->GlobalTransform().GetTranslation();
				object->Transform().Rotation() = m_buildingUIObject->GlobalTransform().GetRotation();

				m_buildingUI->SetInfo(this, object);
				GetScene()->AddObject(object);
			}
		}
	}

	virtual bool IsRemovable() override
	{
		return false;
	}

	virtual void OnUpdate(float dt) override
	{
		m_input->Roll();
		if (m_userId == Global::Get().userId)
			RecordInputAction();
		else
			HidePlayerUI();

		auto skillFlag = 0;
		auto& curSkill = GetCurSkill();
		if (curSkill.Get())
		{
			skillFlag = curSkill->Update(this, dt);
		}

		if (!(skillFlag & Skill::FLAG_NO_MOVE))
		{
			MovePlayer(dt);
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

		if (m_input->m_synchChooseBuildingId != m_curBuildingUiId && m_input->m_synchChooseBuildingId != INVALID_ID)
		{
			SetBuilding(m_input->m_synchChooseBuildingId);
		}

		CheckBuildObjects(dt);
		UseSkill(dt);
		CheckPickItem();
		UpdatePathFinder();
		ActivateNearMonsters();

		if (m_userId == Global::Get().userId)
		{
			Global::Get().ExecuteAction(&m_inputAction);
		}

		m_input->RollSynch();
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

			/*auto bullet = mheap::New<GameObject2D>(GameObject2D::DYNAMIC);
			auto bulletRdr = bullet->NewComponent<SpriteRenderer>("red.png");
			bulletRdr->Sprite().FitTextureSize({ 60, 10 });
			bulletRdr->Sprite().SetAnchorPoint({ 0.5f, 0.5f });

			bullet->NewComponent<FastBulletScript>()->Setup(GetObject(), dir, 3000.0f);*/

			/*bullet->NewComponent<Physics2D>(Physics2D::SENSOR, m_bulletCollider)
				->CollisionMask() = (1ull << 3);*/

			auto& gun = m_guns[m_curGunId];
			auto bullet = gun->NewBullet(this, dir);

			bullet->Position() = Position();
			bullet->Position().x += 25;
			bullet->Position().y += 40;

			bullet->Position() += dir * 50;

			bullet->Rotation() = rotation;
			m_scene->AddObject(bullet);

			m_gunRecoilEnd = m_gun->Position();
			m_gunRecoilBegin = m_gun->Position() - dir * m_gunRecoilLen;
			m_gun->Position() = m_gunRecoilBegin;

			m_recoilMax = gun->GetRecoilTime(this);
			m_recoil = m_recoilMax;
		}
	}

	inline auto& PlayerData()
	{
		return m_data;
	}

	inline auto GetSkills()
	{
		return m_skills;
	}

	inline constexpr auto GetSkillsCount()
	{
		return sizeof(m_skills) / sizeof(m_skills[0]);
	}

	inline auto GetCurSkillId()
	{
		return m_curSkillIdx;
	}

	inline Handle<Skill>& GetCurSkill()
	{
		return m_skills[m_curSkillIdx];
	}

	inline Handle<Renderer2D>& GetCurSkillUI()
	{
		return m_skillsUI[m_curSkillIdx];
	}

	inline Vec2 GetForwardDir()
	{
		auto rotation = m_input->m_synchRotation;
		return { std::cos(rotation), std::sin(rotation) };
	}

	inline auto GetInputRotation()
	{
		return m_input->m_synchRotation;
	}

	inline Vec2 CenterPosition()
	{
		return Position() + Vec2(25, 25);
	}

	inline Physics2D* GetPhysics()
	{
		return GetObject()->GetComponentRaw<Physics2D>();
	}

	inline ID GetTeamId()
	{
		return m_userId % 2;
	}
};