#define HAS_THREADS

#include "thread_pool.hpp"
using namespace Engine;

#ifdef HAS_THREADS

#include <thread>
#include <chrono>
#include <mutex>
#include <optional>
#include <functional>
#include <cstdlib>

constexpr int THREAD_COUNT = 4;
constexpr int TIMEOUT = 100;

static std::vector<std::function<void()>> s_task_queue;
static std::mutex s_task_queue_mutex;

static bool s_should_shutdown = false;
static bool s_shutdown_on_completed = false;
static std::mutex s_should_shutdown_mutex;

static std::vector<std::thread> s_threads;
static bool s_has_threads_started { false };

static std::function<void()> next_task()
{
    std::lock_guard<std::mutex> quard(s_task_queue_mutex);
    if (s_task_queue.empty())
        return {};

    auto task = s_task_queue.back();
    s_task_queue.pop_back();
    return task;
}

static void worker_thread()
{
    for (;;)
    {
        bool shutdown_on_completed;

        {
            std::lock_guard<std::mutex> quard(s_should_shutdown_mutex);
            if (s_should_shutdown)
                break;
            shutdown_on_completed = s_shutdown_on_completed;
        }

        auto task = next_task();
        if (!task)
        {
            if (shutdown_on_completed)
                break;

            std::this_thread::sleep_for(std::chrono::milliseconds(TIMEOUT));
            continue;
        }

        task();
    }
}

static void shutdown()
{
    if (!s_has_threads_started)
        return;

    {
        std::lock_guard<std::mutex> quard(s_should_shutdown_mutex);
        s_should_shutdown = true;
    }

    for (auto &thread : s_threads)
        thread.join();

    s_threads.clear();
    s_has_threads_started = false;
    s_should_shutdown = false;
    s_shutdown_on_completed = false;
}

static void start_threads_if_needed()
{
    if (s_has_threads_started)
        return;

    for (int i = 0; i < THREAD_COUNT; i++)
        s_threads.push_back(std::thread(worker_thread));

    std::atexit(shutdown);
    s_has_threads_started = true;
}

void ThreadPool::queue_task(std::function<void()> task)
{
    start_threads_if_needed();

    std::lock_guard<std::mutex> quard(s_task_queue_mutex);
    s_task_queue.insert(s_task_queue.begin(), task);
}

void ThreadPool::finished_loading()
{
    std::lock_guard<std::mutex> quard(s_should_shutdown_mutex);
    s_shutdown_on_completed = true;
}

#else

void ThreadPool::queue_task(std::function<void()> task)
{
    task();
}

void ThreadPool::finished_loading()
{
}

#endif

