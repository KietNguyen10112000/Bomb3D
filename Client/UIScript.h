#pragma once

#include "Components2D/Script/Script2D.h"
#include "Objects2D/Physics/Colliders/CircleCollider.h"

#include "imgui/imgui.h"
#include "imgui-SFML/imgui-SFML.h"

#include "Global.h"
#include "UIConsole.h"
#include "Monster.h"

#include "GameUtils.h"

#include "GameFont.h"

using namespace soft;

//#include <pybind11/embed.h>
//#include <pybind11/iostream.h>

//namespace py = pybind11;

class UIScript : Traceable<UIScript>, public Script2D
{
protected:
	static constexpr size_t MINIMAP_SIZE = 256;

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

	sf::RenderTexture m_miniMapRT;
	sf::RectangleShape m_miniMapBlock;
	sf::Sprite m_miniMap;

	Skill* m_playerSkills[5] = {};
	Sprite m_playerSkillsIcon[5] = {};
	String m_playerSkillsDesc[5] = {};

	Sprite m_buildingIcon[256] = {};
	bool m_showBuildingChoseUI = false;

public:
	UIScript()
	{
		m_console = UIConsole::New();
		m_miniMapRT.create(MINIMAP_SIZE, MINIMAP_SIZE);
		m_miniMap.setTexture(m_miniMapRT.getTexture());
		m_miniMap.setPosition(10, 600 - MINIMAP_SIZE / 2 - 10);
		RenderMiniMap();
		PrepareBuildingChoseUI();
	}

	~UIScript()
	{
		UIConsole::Delete(m_console);
	}

private:
	void PrepareBuildingChoseUI()
	{
		for (size_t i = 0; i < 256; i++)
		{
			auto ui = BuildingUI::Get(i);
			if (ui)
			{
				auto path = ui->GetUIImagePath();
				auto& sprite = m_buildingIcon[i];
				sprite.Initialize(path, {}, {});
				//sprite.FitTextureSize({ 150,150 }, true);
				//sprite.PrepareRender();
			}
		}
	}

	void RenderMiniMap()
	{
		m_miniMapRT.clear({ 0,0,0,100 });

		auto map = Global::Get().gameMap.m_movable;
		auto h = Global::Get().gameMap.m_height;
		auto w = Global::Get().gameMap.m_width;

		auto cellSize = MINIMAP_SIZE / (float)w;
		m_miniMapBlock.setSize(sf::Vector2f(MINIMAP_SIZE / (float)w, MINIMAP_SIZE / (float)h));
		m_miniMapBlock.setFillColor({ 255,255,255,128 });

		for (size_t y = 0; y < h; y++)
		{
			for (size_t x = 0; x < w; x++)
			{
				auto v = map[y * w + x];
				if (!v)
				{
					m_miniMapBlock.setPosition({ x * cellSize, MINIMAP_SIZE - y * cellSize - cellSize });
					m_miniMapRT.draw(m_miniMapBlock);
				}
			}
		}
	}

	void RenderBuildingChoseUI()
	{
		if (!m_showBuildingChoseUI)
		{
			return;
		}

		bool open = true;
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		//ImGui::SetNextWindowBgAlpha(0.35f);
		ImGui::SetNextWindowSize(ImVec2(700, 500));
		ImGui::Begin("Chose building", &open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

		constexpr size_t NUM_COLUMNS = 4;

		size_t count = 0;
		for (size_t i = 0; i < 256; i++)
		{
			auto& icon = m_buildingIcon[i];
			auto ui = BuildingUI::Get(i);
			if (ui)
			{
				auto row = count / NUM_COLUMNS;
				auto col = count % NUM_COLUMNS;

				
				if (ImGui::ImageButton(icon.SFSprite(), sf::Vector2f(150,150)))
				{
					Global::Get().GetMyPlayer()->SetBuildingSynch(i);
				}

				if (col != NUM_COLUMNS - 1)
				{
					ImGui::SameLine();
				}

				count++;
			}
		}

		ImGui::End();
	}

	void RenderConsole()
	{
		if (!m_console) return;

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

	/*void SpawnMonster(const Vec2& pos)
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

		physics->CollisionMask() = (1ull << 2);

		monster->Position() = pos;
		monster->Tag() = 10;

		GameUtils::AddHpBar(monster, sf::Color::Red, 70, 100, &script->DynamicObjectProperties().hp)
			->Position() = { -35, -40 };

		m_scene->AddObject(monster);
	}*/

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

		/*if (Global::Get().setting.isOnRClickToSpawnMonster && Input()->IsKeyUp('P'))
		{
			auto& cursorPos = Input()->GetCursor().position;
			auto center = Global::Get().cam->GetWorldPosition(Vec2(cursorPos.x, cursorPos.y),
				Input()->GetWindowWidth(), Input()->GetWindowHeight());
			SpawnMonster(center);
		}*/

		if (Input()->IsKeyUp('B'))
		{
			m_showBuildingChoseUI = !m_showBuildingChoseUI;
			Global::Get().setting.isStopPlayerLeftMouse = !Global::Get().setting.isStopPlayerLeftMouse;
		}
	}

	inline void ShowMyPlayerInfo()
	{
		auto player = Global::Get().GetMyPlayer();
		auto& playerData = player->PlayerData();

		bool open = true;
		ImGui::SetNextWindowPos(ImVec2(280, 720 - 55 - 55));
		ImGui::SetNextWindowBgAlpha(0.35f);
		ImGui::SetNextWindowSize(ImVec2(960 - 300, 50));
		ImGui::Begin("Player Info", &open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

		//ImGui::SetNextItemWidth(50);
		ImGui::PushStyleColor(0, { 1,1,0,1 });
		ImGui::TextWrapped("%d coin", playerData.coin);
		ImGui::PopStyleColor();

		ImGui::SameLine();

		ImGui::PushStyleColor(0, { 0,0,1,1 });
		ImGui::TextWrapped("%d exp", player->GetTeam().exp);
		ImGui::PopStyleColor();

		ImGui::End();
	}

	inline void ShowMyPlayerUI()
	{
		ShowMyPlayerInfo();

		auto rdr = m_scene->GetRenderingSystem();

		auto player = Global::Get().GetMyPlayer();
		auto& playerData = player->PlayerData();

		bool open = true;
		ImGui::SetNextWindowPos(ImVec2(280, 720 - 55));
		ImGui::SetNextWindowBgAlpha(0.35f);
		ImGui::SetNextWindowSize(ImVec2(960 - 300, 50));
		ImGui::Begin("Player Skills", &open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

		auto skills = player->GetSkills();
		auto skillCount = player->GetSkillsCount();
		for (size_t i = 0; i < skillCount; i++)
		{
			auto& skill = skills[i];
			if (m_playerSkills[i] != skill.Get())
			{
				auto path = skill->GetGUIImgPath();
				auto desc = skill->GetDesc();
				m_playerSkillsIcon[i].Initialize(path, AARect(), {});
				//m_playerSkillsIcon[i].Transform().Translation() = { 400 + i * 40, 720 - 45 };
				m_playerSkillsIcon[i].FitTextureSize({ 32,32 });
				m_playerSkillsDesc[i] = desc;
			}

			if (skill)
			{
				/*if (skill->m_coolDown != 0)
				{
					m_playerSkillsIcon[i].SetColor(128, 128, 128, 255);
				}*/

				//ImGui::SetCursorPos({ 32.0f * i + 32, 10 });
				ImGui::SameLine();
				m_playerSkillsIcon[i].PrepareRender();
				ImGui::Image(m_playerSkillsIcon[i].SFSprite(), skill->m_coolDown != 0 ? sf::Color(128,128,128,255) : sf::Color::White);

				if (skill->m_coolDown != 0)
				{
					//ImGui::SetCursorPos({ 32.0f * i + 32 + 10, 10 });
					ImGui::SameLine();
					ImGui::Text("%.2f", skill->m_coolDown);
				}
				
			}
		}

		ImGui::End();
	}

	inline void ShowGameOver()
	{
		bool open = true;
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		//ImGui::SetNextWindowBgAlpha(0.35f);
		auto& io = ImGui::GetIO();
		ImGui::SetNextWindowSize(io.DisplaySize);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::Begin("GameOver", &open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
		//ImGui::SetWindowFontScale(5.0f);

		char* text = nullptr;
		if (Global::Get().GetMyTeam().isVictory)
		{
			text = (char*)u8"Thắng";
		}
		else
		{
			text = (char*)u8"Thua";
		}

		auto winSize = ImGui::GetWindowSize();

		ImGui::PushFont(GameFont::s_font100);
		auto textSize= ImGui::CalcTextSize(text);
		ImGui::SetCursorPos(ImVec2((winSize.x - textSize.x) * 0.5f, (winSize.y - textSize.y) * 0.5f - 80));
		ImGui::Text(text);
		ImGui::PopFont();

		text = (char*)u8"Hãy thoát game vào lại để bắt đầu trận đấu mới!";
		ImGui::PushFont(GameFont::s_font50);
		textSize = ImGui::CalcTextSize(text);
		ImGui::SetCursorPos(ImVec2((winSize.x - textSize.x) * 0.5f, (winSize.y - textSize.y) * 0.5f));
		ImGui::Text(text);
		ImGui::PopFont();
	
		ImGui::End();
		ImGui::PopStyleVar();
	}

	inline void ShowRespawnCountDown()
	{
		bool open = true;
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		//ImGui::SetNextWindowBgAlpha(0.35f);
		auto& io = ImGui::GetIO();
		ImGui::SetNextWindowSize(io.DisplaySize);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::Begin("Die", &open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
		//ImGui::SetWindowFontScale(5.0f);

		char countdownStr[32] = {};
		snprintf(countdownStr, 32, u8"Còn %d giây", (size_t)Global::Get().GetMyPlayer()->m_respawnTimer.Remain());
		char* text = countdownStr;

		auto winSize = ImGui::GetWindowSize();

		ImGui::PushFont(GameFont::s_font100);
		auto textSize = ImGui::CalcTextSize(text);
		ImGui::SetCursorPos(ImVec2((winSize.x - textSize.x) * 0.5f, (winSize.y - textSize.y) * 0.5f - 80));
		ImGui::Text(text);
		ImGui::PopFont();

		text = (char*)u8"Bạn đã chết, hãy đợi để hồi sinh!";
		ImGui::PushFont(GameFont::s_font50);
		textSize = ImGui::CalcTextSize(text);
		ImGui::SetCursorPos(ImVec2((winSize.x - textSize.x) * 0.5f, (winSize.y - textSize.y) * 0.5f));
		ImGui::Text(text);
		ImGui::PopFont();

		ImGui::End();
		ImGui::PopStyleVar();
	}

	inline void ShowRemainVictoryTime()
	{
		auto& myTeam = Global::Get().GetMyTeam();
		auto& oppositeTeam = Global::Get().GetOppositeTeam();

		if (!myTeam.hasVictoryTower && !oppositeTeam.hasVictoryTower)
		{
			return;
		}

		constexpr float WIDTH = 150.0f;
		auto& io = ImGui::GetIO();
		bool open = true;
		ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x / 2.0f - WIDTH / 2.0f, -5));
		ImGui::SetNextWindowBgAlpha(0.35f);
		ImGui::SetNextWindowSize(ImVec2(WIDTH, 70));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::Begin("RemainVictoryTime", &open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

		//auto winSize = ImGui::GetWindowSize();

		char countdownStr[32] = {};
		char* text = countdownStr;

		if (myTeam.hasVictoryTower)
		{
			snprintf(countdownStr, 32, u8"%d giây sẽ thắng", (size_t)myTeam.remainVictoryTime);
			//auto textSize = ImGui::CalcTextSize(text);

			ImGui::PushStyleColor(0, { 0,1,0,1 });
			//ImGui::SetCursorPos(ImVec2((winSize.x - textSize.x) * 0.5f, (winSize.y - textSize.y) * 0.5f));
			ImGui::Text(text);
			ImGui::PopStyleColor();
		}

		if (oppositeTeam.hasVictoryTower)
		{
			snprintf(countdownStr, 32, u8"%d giây sẽ thua", (size_t)oppositeTeam.remainVictoryTime);
			//auto textSize = ImGui::CalcTextSize(text);

			ImGui::PushStyleColor(0, { 1,0,0,1 });
			//ImGui::SetCursorPos(ImVec2((winSize.x - textSize.x) * 0.5f, (winSize.y - textSize.y) * 0.5f));
			ImGui::Text(text);
			ImGui::PopStyleColor();
		}

		ImGui::End();
		ImGui::PopStyleVar();
	}

	virtual void OnGUI() override
	{
		auto& window = Graphics2D::Get()->m_window;
		window.draw(m_miniMap);

		auto mapSize = Global::Get().gameMap.m_height;

		m_miniMapBlock.setFillColor({ 0,255,0,128 });
		auto playerPos = Global::Get().GetMyPlayer()->Position();
		auto miniMapPos = (playerPos / (float)(mapSize * GameConfig::CELL_SIZE)) * (float)MINIMAP_SIZE;
		m_miniMapBlock.setPosition(reinterpret_cast<sf::Vector2f&>(miniMapPos) + m_miniMap.getPosition());
		window.draw(m_miniMapBlock);

		ShowMyPlayerUI();

		RenderConsole();
		//UIDebugPathFinder::Render(m_scene->GetRenderingSystem());
		RenderBuildingChoseUI();

		if (Global::Get().isGameOver)
		{
			ShowGameOver();
		}
		else if (Global::Get().GetMyPlayer()->DynamicObjectProperties().hp <= 0.0f)
		{
			ShowRespawnCountDown();
		}

		if (!Global::Get().isGameOver)
		{
			ShowRemainVictoryTime();
		}
	}

public:
	inline void ShowToast(String msg, float howLong, bool now = false)
	{

	}
	
};