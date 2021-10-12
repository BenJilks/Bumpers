#pragma once

#include <chrono>

// #define LOGGER_ENABLED

namespace Engine::Logger
{

#ifdef LOGGER_ENABLED

	void on_frame_start();
	void on_frame_end();

#else

	inline void on_frame_start() {}
	inline void on_frame_end() {}

#endif

}
