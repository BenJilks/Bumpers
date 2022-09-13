/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifdef LOGGER_ENABLED

#include "logger.hpp"
#include <vector>
#include <chrono>
#include <iostream>
#include <map>
#include <numeric>
#include <algorithm>
using namespace Engine;
using namespace std::chrono;

static system_clock::time_point s_second_start;
static system_clock::time_point s_frame_start;
static std::vector<system_clock::duration> s_frames;

void Logger::on_frame_start()
{
	s_frame_start = system_clock::now();
}

float find_percentage(system_clock::duration time)
{
	return (float)duration_cast<milliseconds>(time).count() / 16.0f * 100.0f;
}

void Logger::on_frame_end()
{
	auto now = system_clock::now();
	s_frames.push_back(now - s_frame_start);

	if (duration_cast<seconds>(now - s_second_start).count() >= 1)
	{
		s_second_start = now;
		auto sum = std::accumulate(s_frames.begin(), s_frames.end(), system_clock::duration(0));
		std::cout << "==========================================\n";
		std::cout << "Agerage frame time: " << find_percentage(sum / s_frames.size()) << "%\n";
		std::cout << "Max frame time: " << find_percentage(*std::max_element(s_frames.begin(), s_frames.end())) << "%\n";
		std::cout << "Min frame time: " << find_percentage(*std::min_element(s_frames.begin(), s_frames.end())) << "%\n";
		std::cout << "Dropped frames : " << std::max(60 - (long long)s_frames.size(), 0ll) << "\n";
		std::cout << "==========================================\n\n";
		s_frames.clear();
	}
}

#endif
