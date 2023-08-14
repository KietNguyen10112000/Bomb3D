#pragma once

class ServerConfig
{
public:
	constexpr static size_t MAX_ROOMS = 128;
	constexpr static size_t NUM_SEND_TASK = 2;
	constexpr static size_t NUM_UPDATE_TASK = 2;

	constexpr static size_t NUM_PLAYER_PER_ROOM = 4;

};