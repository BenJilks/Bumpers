/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <functional>

namespace Engine::ThreadPool
{

    void queue_task(std::function<void()> task);
    void finished_loading();

}

