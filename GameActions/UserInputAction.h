#pragma once

#include "Synch/Action.h"

struct UserInput
{
	bool	m_prevKey[256] = {};
	float	m_prevRotation = 0;

	bool	m_curKey[256] = {};
	float	m_curRotation = 0;

	bool	m_synchKey[256] = {};
	float	m_synchRotation = 0;

	inline void Roll()
	{
		m_prevRotation = m_curRotation;
		memcpy(m_prevKey, m_curKey, 256);
	}

	inline void SetKeyDown(byte keyCode)
	{
		m_curKey[keyCode] = true;
	}

	inline void SetKeyUp(byte keyCode)
	{
		m_curKey[keyCode] = false;
	}

	inline void SetKey(byte keyCode, bool isDown)
	{
		m_curKey[keyCode] = isDown;
	}

	inline void SetRotation(float rotation)
	{
		m_curRotation = rotation;
	}

	inline bool IsKeyDown(byte keyCode)
	{
		return m_synchKey[keyCode];
	}
};

class UserInputAction : public Action
{
public:
	ID m_userId = INVALID_ID;

	UserInput* m_input = nullptr;

	byte m_changedKey[256] = {};
	byte m_changedKeyCount = 0;
	float m_synchRotation = 0;

	UserInputAction();

	inline static Action* Initializer()
	{
		return new UserInputAction();
	}

	inline static void Finalizer(Action* a)
	{
		delete ((UserInputAction*)a);
	}

	// Inherited via Action
	virtual void Serialize(ByteStream& stream) override;
	virtual void Deserialize(ByteStreamRead& stream) override;
	virtual void Activate(Scene2D* scene) override;

public:
	inline void SetUserId(ID id, UserInput* input)
	{
		m_userId = id;
		m_input = input;
	}

};