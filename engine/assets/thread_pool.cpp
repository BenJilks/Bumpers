/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "thread_pool.hpp"
#include <chrono>
#include <optional>
#include <functional>
#include <cstdlib>
using namespace Engine;

#ifdef WIN32

#include <Windows.h>

static HANDLE s_task_queue_mutex;
static HANDLE s_should_shutdown_mutex;
static std::vector<HANDLE> s_threads;

class WindowsMutexGuard
{
public:
    WindowsMutexGuard(HANDLE mutex)
        : m_mutex(mutex)
    {
        WaitForSingleObject(mutex, INFINITE);
    }

    ~WindowsMutexGuard()
    {
        ReleaseMutex(m_mutex);
    }

private:
    HANDLE m_mutex;

};

#define MUTEX_GUARD(mutex) \
    WindowsMutexGuard guard(mutex)

#else

#include <thread>
#include <mutex>

static std::mutex s_task_queue_mutex;
static std::mutex s_should_shutdown_mutex;
static std::vector<std::thread> s_threads;

#define MUTEX_GUARD(_mutex) \
    std::lock_guard<std::mutex> guard(_mutex)

#endif

constexpr int THREAD_COUNT = 4;
constexpr int TIMEOUT = 100;

static std::vector<std::function<void()>> s_task_queue;
static bool s_should_shutdown = false;
static bool s_shutdown_on_completed = false;
static bool s_has_threads_started { false };

static std::function<void()> next_task()
{
    MUTEX_GUARD(s_task_queue_mutex);
    if (s_task_queue.empty())
        return {};

    auto task = s_task_queue.back();
    s_task_queue.pop_back();
    return task;
}

#ifdef WIN32
static DWORD WINAPI worker_thread(__in LPVOID)
#else
static void worker_thread()
#endif
{
    for (;;)
    {
        bool shutdown_on_completed;

        {
            MUTEX_GUARD(s_should_shutdown_mutex);
            if (s_should_shutdown)
                break;
            shutdown_on_completed = s_shutdown_on_completed;
        }

        auto task = next_task();
        if (!task)
        {
            if (shutdown_on_completed)
                break;

#ifdef WIN32
            Sleep(TIMEOUT);
#else
            std::this_thread::sleep_for(std::chrono::milliseconds(TIMEOUT));
#endif
            continue;
        }

        task();
    }

#ifdef WIN32
    return 0;
#endif
}

static void shutdown()
{
    if (!s_has_threads_started)
        return;

    {
        MUTEX_GUARD(s_should_shutdown_mutex);
        s_should_shutdown = true;
    }

#ifdef WIN32
    CloseHandle(s_task_queue_mutex);
    CloseHandle(s_should_shutdown_mutex);

    for (auto& thread : s_threads)
    {
        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);
    }
#else
    for (auto &thread : s_threads)
        thread.join();
#endif

    s_threads.clear();
    s_has_threads_started = false;
    s_should_shutdown = false;
    s_shutdown_on_completed = false;
}

static void start_threads_if_needed()
{
    if (s_has_threads_started)
        return;

#ifdef WIN32
    s_task_queue_mutex = CreateMutex(NULL, FALSE, NULL);
    s_should_shutdown_mutex = CreateMutex(NULL, FALSE, NULL);

    for (int i = 0; i < THREAD_COUNT; i++)
        s_threads.push_back(CreateThread(0, 0, worker_thread, 0, 0, NULL));
#else
    for (int i = 0; i < THREAD_COUNT; i++)
        s_threads.push_back(std::thread(worker_thread));
#endif

    std::atexit(shutdown);
    s_has_threads_started = true;
}

void ThreadPool::queue_task(std::function<void()> task)
{
    start_threads_if_needed();

    MUTEX_GUARD(s_task_queue_mutex);
    s_task_queue.insert(s_task_queue.begin(), task);
}

void ThreadPool::finished_loading()
{
    MUTEX_GUARD(s_should_shutdown_mutex);
    s_shutdown_on_completed = true;
}
