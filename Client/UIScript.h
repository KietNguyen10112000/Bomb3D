#pragma once

#include "Components2D/Script/Script2D.h"

#include "imgui/imgui.h"

#include "Global.h"
#include "UIConsole.h"

using namespace soft;

#include <pybind11/embed.h>
#include <pybind11/iostream.h>

namespace py = pybind11;

class UIScript : Traceable<UIScript>, public Script2D, public UIConsole
{
protected:
	SCRIPT2D_DEFAULT_METHOD(UIScript);
	using Base = Script2D;
	TRACEABLE_FRIEND();
	void Trace(Tracer* tracer)
	{
		Base::Trace(tracer);
	}

	bool m_showedDebugUI = false;

private:
	void RenderConsole()
	{
		if (!Global::Get().setting.isOnConsole)
		{
			return;
		}

		UIConsole::RenderConsole(nullptr);
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

public:
	virtual void OnStart() override
	{

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
	}

	virtual void OnGUI() override
	{
		RenderConsole();
	}
	
};