/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

// #define LOGGER_ENABLED

namespace Engine::Logger {

#ifdef LOGGER_ENABLED

void on_frame_start();
void on_frame_end();

#else

inline void on_frame_start() { }
inline void on_frame_end() { }

#endif

}
