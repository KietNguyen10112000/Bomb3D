#pragma once

#include "Synch/Action.h"
#include "UserInputAction.h"

class GameActions
{
public:
	enum ACTION_ID
	{
		USER_INPUT
	};

	inline static void InitializeAllGameActions()
	{
		ActionCreator::Set(USER_INPUT, UserInputAction::Initializer, UserInputAction::Finalizer);
	}
};

