// framework_threadpool.

#ifndef _FRAMEWORK_THREADPOOL_HPP
#define _FRAMEWORK_THREADPOOL_HPP
#include <stdexcept>

#include "../improfx_log/framework_log.hpp"

#define SYSMD_LB_THREAD std::string("[thread_pool]: ")

/*
* EasyWorksPool(改), https://github.com/rcszc/EasyWorksPool
* Source: RCSZ 2023_08_28, Modify: RCSZ 2023_11_08.
* ImThreadTask, system_rtti, error.
*/
namespace ImThreadTask {

    namespace system_rtti {
        struct tpc_object {
            std::string object_name;
            std::size_t object_hash;
        };

        template<typename nClass>
        tpc_object TASKOBJECTINFO(nClass TaskObject) {
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
        class ewp_error :public std::exception {
        protected:
            std::string error_message = {};
        public:
            ewp_error(ERRINFO message) : error_message(message) {}

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

        void thread_task_execute(uint32_t threads);
        void thread_task_free();

        bool thread_stop_flag = false;

    public:
        ThreadPool(uint32_t InitSetWorkers) {
            thread_task_execute(InitSetWorkers);
        };
        ~ThreadPool() {
            thread_task_free();
        };
        // current creation objectinfo.
        system_rtti::tpc_object pool_objectinfo = {};

        template<typename InClass, typename... Args_param>
        std::future<std::shared_ptr<InClass>> PushTask(Args_param... Args) {
            auto push_task = std::make_shared<std::packaged_task<std::shared_ptr<InClass>()>>(
                [Args = std::make_tuple(std::forward<Args_param>(Args)...), this]() mutable {
                    try {
                        return std::apply([](auto&&... Args) {
                            return std::make_shared<InClass>(std::forward<decltype(Args)>(Args)...);
                        }, std::move(Args));
                    }
                    catch (...) {
                        throw error::ewp_error("failed create object.");
                        return std::shared_ptr<InClass>(nullptr);
                    }
                });
            // create object => get object info.
            pool_objectinfo = system_rtti::TASKOBJECTINFO(push_task);

            std::future<std::shared_ptr<InClass>> result_async = push_task->get_future();
            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                if (thread_stop_flag) {
                    // disable push task.
                    throw error::ewp_error("failed thread pool stop.");
                    return result_async;
                }
                else
                    pool_tasks.emplace([push_task]() { (*push_task)(); });
            }
            workers_condition.notify_one();

            return result_async;
        }

        uint32_t QueueTasksCount();
        void     ResizeWorkers(uint32_t ResizeThreads);
    };

    size_t getthis_thread_id_number();
}

#endif