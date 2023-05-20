#pragma once

#include "Synch/Action.h"

#include "MatchStartAction.h"
#include "UserInputAction.h"

class GameActions
{
public:
	enum ACTION_ID
	{
		MATCH_START,
		USER_INPUT,
	};

	inline static void InitializeAllGameActions()
	{
		ActionCreator::Set(MATCH_START,		MatchStartAction::Initializer,		MatchStartAction::Finalizer);
		ActionCreator::Set(USER_INPUT,		UserInputAction::Initializer,		UserInputAction::Finalizer);
	}
};

