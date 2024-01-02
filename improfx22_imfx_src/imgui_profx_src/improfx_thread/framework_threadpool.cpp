// framework_threadpool.
#include "framework_threadpool.hpp"

using namespace std;

namespace ImThreadTask {

    void ThreadingPool::ThreadsTaskExecution(uint32_t ThreadsNum) {
        // start threads(workers).
        for (size_t i = 0; i < ThreadsNum; ++i) {
            try {
                ThreadWorkers.emplace_back([this] {
                    // loop execution task.
                    while (true) {
                        function<void()> WorkTaskExecution;
                        {
                            unique_lock<mutex> Lock(PoolMutex);
                            WorkersCondition.wait(Lock, [this] { return PoolStopFlag || !PoolTasks.empty(); });
                            // loop exit.
                            if (PoolStopFlag && PoolTasks.empty())
                                break;
                            WorkTaskExecution = move(PoolTasks.front());
                            // queue => delete task.
                            PoolTasks.pop();
                        }
                        ++WorkingThreadsCount;
                        WorkTaskExecution();
                        --WorkingThreadsCount;
                    }
                });
            }
            catch (...) {
                throw Error::TPerror("failed create thread.");
            }
        }
    }

    void ThreadingPool::ThreadsTaskFree() {
        {
            unique_lock<mutex> Lock(PoolMutex);
            PoolStopFlag = true;
        }
        try {
            WorkersCondition.notify_all();
            for (thread& Worker : ThreadWorkers) {
                // free all threads.
                Worker.join();
            }
        }
        catch (...) {
            throw Error::TPerror("failed delete thread.");
        }
    }

    uint32_t ThreadingPool::GetQueueTasks() {
        uint32_t TasksCount = NULL;
        {
            unique_lock<mutex> Lock(PoolMutex);
            TasksCount = (uint32_t)PoolTasks.size();
        }
        return TasksCount;
    }

    uint32_t ThreadingPool::GetWorkingThreads() {
        return WorkingThreadsCount;
    }

    void ThreadingPool::ResizeWorkers(uint32_t ResizeThreads) {
        {
            unique_lock<mutex> Lock(PoolMutex);
            WorkersCondition.notify_all();
        }
        ThreadsTaskFree();

        ThreadWorkers.resize(ResizeThreads);
        PoolStopFlag = false;

        ThreadsTaskExecution(ResizeThreads);
    }

    size_t ThisThreadCountID() {
        size_t ThisThreadIDcount = NULL;

        thread::id ThreadID = this_thread::get_id();
        hash<thread::id> Hasher;
        ThisThreadIDcount = Hasher(ThreadID);

        return ThisThreadIDcount;
    }
}