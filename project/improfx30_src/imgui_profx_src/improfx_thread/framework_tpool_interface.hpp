// framework_tpool_interface.

#ifndef _FRAMEWORK_TPOOL_INTERFACE_HPP
#define _FRAMEWORK_TPOOL_INTERFACE_HPP
#include <stdexcept>

#include "../improfx_log/framework_log.hpp"

// [2024_01_05](1): ImProFx框架线程池接口.
namespace IFC_THPOOL {
    // run_time_type_information.
    struct RTTI_OBJINFO {

        std::string ObjectName;
        std::size_t ObjectHash;
    };

    class WorkPoolBase {
    public:
        virtual RTTI_OBJINFO GetCreateObjectInfo() = 0;

        virtual uint32_t GetWorkingThreadsCount() = 0;
        virtual uint32_t GetTaskQueueCount() = 0;
        virtual void ResizeWorkers(uint32_t resize) = 0;

        virtual ~WorkPoolBase() = default;
    };
}

#endif