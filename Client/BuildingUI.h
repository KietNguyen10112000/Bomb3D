#pragma once

#include "Core/Structures/String.h"
#include "Math/Math.h"

namespace soft
{
	class GameObject2D;
}

using namespace soft;

class PlayerScript;

class BuildingUI
{
private:
	static BuildingUI* s_instances[256];

public:
	static void Initialize();
	static void Finalize();

	inline static auto Get(size_t id)
	{
		return s_instances[id];
	}

	virtual ~BuildingUI() {};

	virtual String GetUIImagePath() = 0;
	virtual String GetUIName() = 0;
	virtual String GetDesc() 
	{
		return u8"Chưa có mô tả cho công trình này";
	};

	// return prepare distance
	virtual float PrepareUI(GameObject2D* object, PlayerScript* player) = 0;
	virtual bool IsAllowRotation() = 0;
	virtual bool CheckCanBuild(const Transform2D& transform, PlayerScript* player) = 0;
	virtual size_t GetBuildingObjectGeneratorId() = 0;
	virtual void SetInfo(PlayerScript* player, GameObject2D* building) = 0;

};