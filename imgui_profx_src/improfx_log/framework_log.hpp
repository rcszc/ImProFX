// framework_log. [2023_10_11] RCSZ.

#ifndef _FRAMEWORK_LOG_HPP
#define _FRAMEWORK_LOG_HPP
#include <iostream>
#include <mutex>
#include <sstream>
#include <chrono>

#include "../framework_define.hpp"

#define LOG_ERR  "[ERROR]"   // 标签 <错误>
#define LOG_WARN "[WARRING]" // 标签 <警告>
#define LOG_INFO "[INFO]"    // 标签 <信息>
#define LOG_TRC  "[TRACE]"   // 标签 <跟踪>
#define LOG_PERF "[PERF]"    // 标签 <性能>

#define SYSTEM_MODULE_LABLE_log std::string("[log_system]: ")

namespace LOGCONS {
	/*
	* @param  string, bool
	* @return void
	*/
	void LogInfoPush(const char* Lable, std::string LogStrText);
	/*
	* @param  void
	* @return Vector3T<size_t> LogLines x : Lines, y : warring, z : error
	*/
	Vector3T<size_t> LogLinesNumber();

	namespace ReadLogCache {
		/*
		* 截取日志段.
		* @param  uint32_t [End, End - LINES]
		* @return string
		*/
		std::string ExtractLogSegment(uint32_t Lines);
	}
	// get src time[sec].
	size_t GetTimeNow();
}

namespace LOGFILE {
	// async process. write folder, timer.
	bool StartLogFileProcess(const char* folder, int64_t timer);
	bool FreeLogFileProcess();

	bool LogFileStatus();
}

#endif