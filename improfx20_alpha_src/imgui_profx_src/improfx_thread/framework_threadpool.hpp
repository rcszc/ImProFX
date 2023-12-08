// framework_threadpool.

#ifndef _FRAMEWORK_THREADPOOL_HPP
#define _FRAMEWORK_THREADPOOL_HPP
#include <stdexcept>

#include "../improfx_log/framework_log.hpp"

#define SYSMD_LB_THREAD "[im_threadpool]: "

/*
* EasyWorksPool(改), https://github.com/rcszc/EasyWorksPool
* Source: RCSZ 2023_08_28, Modify: RCSZ 2023_11_08.
* ImThreadTask, system_objectinfo, error.
*/
namespace ImThreadTask {
    namespace system_objectinfo {

        struct tpc_object {
            std::string object_name;
            std::size_t object_hash;
        };

        template<typename nClass>
        tpc_object TaskObjectInfo(nClass TaskObject) {
            tpc_object ResultInfo = {};

            // RunTime Type Identification [RTTI].
            const std::type_info& object_info = typeid(TaskObject);

            ResultInfo.object_name = object_info.name();
            ResultInfo.object_hash = object_info.hash_code();

            return ResultInfo;
        }
    }

    namespace error {
        typedef const char* ERRINFO;

        // 异常处理, std::exception::what.
        class TPerror :public std::exception {
        protected:
            std::string error_message = {};
        public:
            TPerror(ERRINFO message) : error_message(message) {}

            ERRINFO what() const noexcept override {
                return error_message.c_str();
            }
        };
    }

    class ThreadPool {
    protected:
        std::vector<std::thread>          thread_workers;
        std::queue<std::function<void()>> pool_tasks;
        std::mutex                        queue_mutex;
        std::condition_variable           workers_condition;
        std::atomic_uint32_t              working_threads_count{0};

        void thread_task_execute(uint32_t threads);
        void thread_task_free();

        bool thread_stop_flag = false;

    public:
        ThreadPool(uint32_t InitSetWorkers) {
            thread_task_execute(InitSetWorkers);
            LOGCONS::PushLogger(LOG_INFO, SYSMD_LB_THREAD, "create task workers: %u" + InitSetWorkers);
        };
        ~ThreadPool() {
            thread_task_free();
            LOGCONS::PushLogger(LOG_INFO, SYSMD_LB_THREAD, "free task workers.");
        };
        // current creation objectinfo.
        system_objectinfo::tpc_object ObjectInfo = {};

        template<typename InClass, typename... ArgsParam>
        std::future<std::shared_ptr<InClass>> PushTask(ArgsParam... Args) {
            auto push_task = std::make_shared<std::packaged_task<std::shared_ptr<InClass>()>>(
                [Args = std::make_tuple(std::forward<ArgsParam>(Args)...), this]() mutable {
                    try {
                        return std::apply([](auto&&... Args) {
                            return std::make_shared<InClass>(std::forward<decltype(Args)>(Args)...);
                        }, std::move(Args));
                    }
                    catch (...) {
                        throw error::TPerror("failed create object.");
                        return std::shared_ptr<InClass>(nullptr);
                    }
                });
            // create object => get object info.
            ObjectInfo = system_objectinfo::TaskObjectInfo(push_task);

            std::future<std::shared_ptr<InClass>> result_async = push_task->get_future();
            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                if (thread_stop_flag) {
                    // disable push task.
                    throw error::TPerror("failed thread pool stop.");
                    return result_async;
                }
                else
                    pool_tasks.emplace([push_task]() { (*push_task)(); });
            }
            workers_condition.notify_one();

            return result_async;
        }

        uint32_t QueueTasksCount();
        uint32_t WorkingThreadsCount();
        void     ResizeWorkers(uint32_t ResizeThreads);

    };

    size_t GetThisThreadIDcount();
}

#endif