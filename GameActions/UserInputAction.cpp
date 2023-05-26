#include "UserInputAction.h"
#include "GameActions.h"

#ifdef _CLIENT
#include "Client/Global.h"
#include "Client/GameLoopHandler.h"
#endif // _CLIENT

UserInputAction::UserInputAction() : Action(GameActions::USER_INPUT)
{
}

void UserInputAction::Serialize(ByteStream& stream)
{
	if (m_input)
	{
		// client side serialize
		stream.Put<ID>(m_userId);
		auto pNumChangedKey = stream.Put<byte>(0);

		size_t numChanged = 0;
		for (size_t i = 0; i < 256; i++)
		{
			if (m_input->m_curKey[i] != m_input->m_prevKey[i])
			{
				stream.Put<byte>((byte)i);
				numChanged++;
			}
		}

		stream.Set(pNumChangedKey, (byte)numChanged);
		stream.Put<float>(m_input->m_curRotation);
		return;
	}

	// server side serialize
	stream.Put<ID>(m_userId);
	stream.Put<byte>(m_changedKeyCount);

	size_t numChanged = m_changedKeyCount;
	for (size_t i = 0; i < numChanged; i++)
	{
		stream.Put<byte>(m_changedKey[i]);
	}

	stream.Put<float>(m_synchRotation);
}

void UserInputAction::Deserialize(ByteStreamRead& stream)
{
	m_userId = stream.Get<ID>();
	size_t pNumChangedKey = stream.Get<byte>();

	for (size_t i = 0; i < pNumChangedKey; i++)
	{
		auto keyCode = stream.Get<byte>();
		m_changedKey[m_changedKeyCount++] = keyCode;
	}

	m_synchRotation = stream.Get<float>();
}

void UserInputAction::Activate(Scene2D* scene)
{
#ifdef _CLIENT
	auto input = &Global::Get().gameLoop->m_userInput[m_userId];
	input->m_synchRotation = m_synchRotation;
	for (size_t i = 0; i < (size_t)m_changedKeyCount; i++)
	{
		auto keyCode = m_changedKey[i];
		input->m_synchKey[keyCode] = !input->m_synchKey[keyCode];
	}
#endif
}
