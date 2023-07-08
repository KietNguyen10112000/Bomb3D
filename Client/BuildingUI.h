#pragma once

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

	virtual void PrepareUI(GameObject2D* object) = 0;
	virtual bool IsAllowRotation() = 0;
	virtual bool IsAllowOnNonMovable() = 0;
	virtual size_t GetBuildingObjectGeneratorId() = 0;
	virtual void SetInfo(PlayerScript* player, GameObject2D* building) = 0;

};