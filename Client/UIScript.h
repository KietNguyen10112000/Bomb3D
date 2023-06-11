#pragma once

#include "Components2D/Script/Script2D.h"
#include "Objects2D/Physics/Colliders/CircleCollider.h"

#include "imgui/imgui.h"

#include "Global.h"
#include "UIConsole.h"
#include "UIDebugPathFinder.h"
#include "Monster.h"

using namespace soft;

//#include <pybind11/embed.h>
//#include <pybind11/iostream.h>

//namespace py = pybind11;

class UIScript : Traceable<UIScript>, public Script2D, public UIDebugPathFinder
{
protected:
	SCRIPT2D_DEFAULT_METHOD(UIScript);
	using Base = Script2D;
	TRACEABLE_FRIEND();
	void Trace(Tracer* tracer)
	{
		Base::Trace(tracer);
	}

	UIConsole* m_console = nullptr;
	SharedPtr<CircleCollider> m_circleCollider;

	bool m_showedDebugUI = false;

public:
	UIScript()
	{
		m_console = UIConsole::New();
	}

	~UIScript()
	{
		UIConsole::Delete(m_console);
	}

private:
	void RenderConsole()
	{
		m_console->m_isCaptureStdOut = Global::Get().setting.isCaptureSTDCout;
		if (!Global::Get().setting.isOnConsole)
		{
			return;
		}

		m_console->RenderConsole(nullptr);
	}

	void OnShowDebugUI()
	{
		Global::Get().setting.isOnConsole = true;
		Global::Get().setting.playerControlMode = GameSetting::PlayerControlMode::DEBUG;
	}

	void OnHideDebugUI()
	{
		Global::Get().setting.isOnConsole = false;
		Global::Get().setting.playerControlMode = GameSetting::PlayerControlMode::NONE;
	}

	void SpawnMonster(const Vec2& pos)
	{
		if (!Global::Get().gameMap.IsMovable(pos))
		{
			return;
		}

		std::cout << "SpawnMonster\n";

		auto monster	= mheap::New<GameObject2D>(GameObject2D::DYNAMIC);
		auto renderer	= monster->NewComponent<SpritesRenderer>();
		auto physics	= monster->NewComponent<RigidBody2D>(RigidBody2D::DYNAMIC, m_circleCollider);
		auto script		= monster->NewComponent<Monster>();

		renderer->Load("ui/circle-red.png", {}, Vec2(60,60));
		renderer->Sprite(0).SetAnchorPoint({ 0.5f,0.5f });
		
		renderer->Load("ui/circle-green.png", {}, Vec2(60, 60));
		renderer->Sprite(1).SetAnchorPoint({ 0.5f,0.5f });

		renderer->SetSprite(0);
		renderer->ClearAABB();

		monster->Position() = pos;
		m_scene->AddObject(monster);
	}

public:
	virtual void OnStart() override
	{
		m_circleCollider = MakeShared<CircleCollider>(Vec2(0,0), 30.0f);
	}

	virtual void OnUpdate(float dt) override
	{
		// '`~' key
		if (Input()->IsKeyPressed(192))
		{
			if (m_showedDebugUI)
			{
				OnHideDebugUI();
			}
			else
			{
				OnShowDebugUI();
			}
			m_showedDebugUI = !m_showedDebugUI;
		}

		if (Global::Get().setting.isOnRClickToSpawnMonster && Input()->IsKeyPressed('P'))
		{
			auto& cursorPos = Input()->GetCursor().position;
			auto center = Global::Get().cam->GetWorldPosition(Vec2(cursorPos.x, cursorPos.y),
				Input()->GetWindowWidth(), Input()->GetWindowHeight());
			SpawnMonster(center);
		}
	}

	virtual void OnGUI() override
	{
		RenderConsole();
		UIDebugPathFinder::Render(m_scene->GetRenderingSystem());
	}
	
};