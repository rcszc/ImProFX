// framework_threadpool.
#include "framework_threadpool.hpp"

using namespace std;

namespace ImThreadTask {

    void ThreadPool::thread_task_execute(uint32_t num_threads) {
        // start threads(workers).
        for (size_t i = 0; i < num_threads; ++i) {
            try {
                thread_workers.emplace_back([this] {
                    // loop execution task.
                    while (true) {
                        function<void()> work_task;
                        {
                            unique_lock<mutex> lock(queue_mutex);
                            workers_condition.wait(lock, [this] { return thread_stop_flag || !pool_tasks.empty(); });
                            // loop exit.
                            if (thread_stop_flag && pool_tasks.empty())
                                break;
                            work_task = move(pool_tasks.front());
                            // queue => delete task.
                            pool_tasks.pop();
                        }
                        ++working_threads_count;
                        work_task();
                        --working_threads_count;
                    }
                });
            }
            catch (...) {
                throw error::TPerror("failed create thread.");
            }
        }
    }

    void ThreadPool::thread_task_free() {
        {
            unique_lock<mutex> lock(queue_mutex);
            thread_stop_flag = true;
        }
        try {
            workers_condition.notify_all();
            for (thread& worker : thread_workers) {
                // free all threads.
                worker.join();
            }
        }
        catch (...) {
            throw error::TPerror("failed delete thread.");
        }
    }

    uint32_t ThreadPool::QueueTasksCount() {
        uint32_t tasks_count = NULL;
        {
            unique_lock<mutex> _lock(queue_mutex);
            tasks_count = (uint32_t)pool_tasks.size();
        }
        return tasks_count;
    }

    uint32_t ThreadPool::WorkingThreadsCount() {
        return working_threads_count;
    }

    void ThreadPool::ResizeWorkers(uint32_t resize_threads) {
        {
            unique_lock<mutex> _lock(queue_mutex);
            workers_condition.notify_all();
        }
        thread_task_free();

        thread_workers.resize(resize_threads);
        thread_stop_flag = false;

        thread_task_execute(resize_threads);
    }

    size_t GetThisThreadIDcount() {
        size_t this_thread_id_count = NULL;

        thread::id threadId = this_thread::get_id();
        hash<thread::id> hasher;
        this_thread_id_count = hasher(threadId);

        return this_thread_id_count;
    }
}