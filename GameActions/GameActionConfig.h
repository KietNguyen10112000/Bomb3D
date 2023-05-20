#pragma once



struct GameActionConfig
{
	/*static GameActionConfig s_instance;
	inline static auto& Get()
	{
		return s_instance;
	}*/

	struct ServerConfig
	{
		constexpr static size_t NUM_TICKS_PER_SYNCH = 4;
	};

	struct ClientConfig
	{
		constexpr static size_t NUM_TICKS_PER_SYNCH = 4;
	};

	constexpr static size_t MAX_CLIENTS = 10;
};