#pragma once

#include "imgui/imgui.h"

class GameFont
{
public:
	inline static ImFont* s_font100 = nullptr;
	inline static ImFont* s_font50 = nullptr;

public:
	inline static void Initialize()
	{
		ImGuiStyle& style = ImGui::GetStyle();

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImFontConfig config{};
		config.OversampleH = config.OversampleV = 1;
		config.PixelSnapH = true;
		config.SizePixels = 13.0f * 1.0f;
		config.GlyphOffset.y = 1.0f * ((float)(int)(((config.SizePixels / 13.0f)) + 0.5f));
		config.GlyphRanges = io.Fonts->GetGlyphRangesVietnamese();

		s_font100 = io.Fonts->AddFontFromFileTTF("segoeui.ttf", (int)(100.0f), &config);
		s_font50 = io.Fonts->AddFontFromFileTTF("segoeui.ttf", (int)(50.0f), &config);
	}

};