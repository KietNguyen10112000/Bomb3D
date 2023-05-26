#pragma once

#include "Core/Structures/Raw/ConcurrentQueue.h"

#include "Package.h"

#include "TaskSystem/TaskSystem.h"

class Action;

using ActionInitializer		= Action* (*)();
using ActionFinalizer		= void (*)(Action*);

using ActionID = uint32_t;

namespace soft 
{
	class Scene2D;
}

using namespace soft;

class Action
{
private:
	friend class ActionQueue;

	ActionID m_id = (ActionID)INVALID_ID;

public:
	inline Action(ActionID id) : m_id(id) {};

	virtual void Serialize(ByteStream& stream) = 0;
	virtual void Deserialize(ByteStreamRead& stream) = 0;
	virtual void Activate(Scene2D* scene) = 0;

public:
	inline const auto GetActionID() const
	{
		return m_id;
	}

};

class ActionCreator
{
public:
	constexpr static size_t MAX_ACTIONS = 1024;

	inline static ActionInitializer			s_initializer		[MAX_ACTIONS] = {};
	inline static ActionFinalizer			s_finalizer			[MAX_ACTIONS] = {};

	inline static auto New(ActionID id)
	{
		return s_initializer[id]();
	}

	inline static void Set(ActionID id, ActionInitializer deserializer, ActionFinalizer deleter)
	{
		s_initializer[id] = deserializer;
		s_finalizer[id] = deleter;
	}

	inline static void Delete(Action* action)
	{
		s_finalizer[action->GetActionID()](action);
	}
};