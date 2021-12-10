#pragma once

#include <functional>

namespace Engine::ThreadPool
{

    void queue_task(std::function<void()> task);
    void finished_loading();

}

