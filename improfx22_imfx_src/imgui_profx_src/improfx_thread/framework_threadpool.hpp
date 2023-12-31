// framework_threadpool.

#ifndef _FRAMEWORK_THREADPOOL_HPP
#define _FRAMEWORK_THREADPOOL_HPP
#include <stdexcept>

#include "../improfx_log/framework_log.hpp"

#define SYSMD_LB_THREAD "[im_threadpool]: "

/*
* EasyWorksPool(改), https://github.com/rcszc/EasyWorksPool
* Source: RCSZ 2023_08_28, Modify: RCSZ 2023_11_08.
* ImThreadTask, SYSTEMOBJINFO, Error.
*/
namespace ImThreadTask {
    namespace SYSTEMOBJINFO {

        struct RTTI_OBJINFO {
            std::string ObjectName;
            std::size_t ObejctHash;
        };

        template<typename nClass>
        RTTI_OBJINFO TASK_OBJECT_INFO(nClass TASK_OBJ) {
            RTTI_OBJINFO ResultInfo = {};

            // RunTime Type Identification [RTTI].
            const std::type_info& GetInfo = typeid(TASK_OBJ);

            ResultInfo.ObjectName = GetInfo.name();
            ResultInfo.ObejctHash = GetInfo.hash_code();

            return ResultInfo;
        }
    }

    namespace Error {
        typedef const char* ERRINFO;

        // 异常处理, std::exception::what.
        class TPerror :public std::exception {
        protected:
            std::string ErrorMessage = {};
        public:
            TPerror(ERRINFO message) : ErrorMessage(message) {}

            ERRINFO what() const noexcept override {
                return ErrorMessage.c_str();
            }
        };
    }

    class ThreadingPool {
    protected:
        std::vector<std::thread>          ThreadWorkers;
        std::queue<std::function<void()>> PoolTasks;
        std::mutex                        PoolMutex;
        std::condition_variable           WorkersCondition;
        std::atomic_uint32_t              WorkingThreadsCount{NULL};

        void ThreadsTaskExecution(uint32_t ThreadsNum);
        void ThreadsTaskFree();

        // current creation objectinfo.
        SYSTEMOBJINFO::RTTI_OBJINFO ObjectInfo = {};
        bool PoolStopFlag = false;

    public:
        ThreadingPool(uint32_t InitSetWorkers) {
            ThreadsTaskExecution(InitSetWorkers);
            LOGCONS::PushLogger(LOG_INFO, SYSMD_LB_THREAD, "create task workers: %u" + InitSetWorkers);
        };
        ~ThreadingPool() {
            ThreadsTaskFree();
            LOGCONS::PushLogger(LOG_INFO, SYSMD_LB_THREAD, "free task workers.");
        };
        // thread_pool: push => tasks queue.
        template<typename InClass, typename... ArgsParam>
        std::future<std::shared_ptr<InClass>> PushTask(ArgsParam... Args) {
            auto PushTaskObj = std::make_shared<std::packaged_task<std::shared_ptr<InClass>()>>(
                [Args = std::make_tuple(std::forward<ArgsParam>(Args)...), this]() mutable {
                    try {
                        return std::apply([](auto&&... Args) {
                            return std::make_shared<InClass>(std::forward<decltype(Args)>(Args)...);
                        }, std::move(Args));
                    }
                    catch (...) {
                        throw Error::TPerror("failed create object.");
                        return std::shared_ptr<InClass>(nullptr);
                    }
                });
            // create object => get object info.
            ObjectInfo = SYSTEMOBJINFO::TASK_OBJECT_INFO(PushTaskObj);

            std::future<std::shared_ptr<InClass>> result_async = PushTaskObj->get_future();
            {
                std::unique_lock<std::mutex> Lock(PoolMutex);
                if (PoolStopFlag) {
                    // disable push task.
                    throw Error::TPerror("failed thread pool stop.");
                    return result_async;
                }
                else
                    PoolTasks.emplace([PushTaskObj]() { (*PushTaskObj)(); });
            }
            WorkersCondition.notify_one();

            return result_async;
        }

        SYSTEMOBJINFO::RTTI_OBJINFO GetTaskObjectInfo() {
            std::unique_lock<std::mutex> Lock(PoolMutex);
            return ObjectInfo;
        }
        uint32_t GetQueueTasks();
        uint32_t GetWorkingThreads();
        void     ResizeWorkers(uint32_t ResizeThreads);

    };

    size_t ThisThreadCountID();
}

#endif