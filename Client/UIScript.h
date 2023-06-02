#pragma once

#include "Components2D/Script/Script2D.h"

#include "imgui/imgui.h"

using namespace soft;

class UIScript : Traceable<UIScript>, public Script2D
{
protected:
	SCRIPT2D_DEFAULT_METHOD(UIScript);
	using Base = Script2D;
	TRACEABLE_FRIEND();
	void Trace(Tracer* tracer)
	{
		Base::Trace(tracer);
	}

public:
	virtual void OnGUI()
	{
		ImGui::Begin(u8"Xin chào");
		ImGui::Button(u8"Bấm");
		ImGui::End();
	}
	
};