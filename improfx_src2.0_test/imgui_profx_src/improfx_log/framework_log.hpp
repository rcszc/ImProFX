// framework_log. [2023_10_11] RCSZ.

#ifndef _FRAMEWORK_LOG_HPP
#define _FRAMEWORK_LOG_HPP
#include <iostream>
#include <sstream>
#include <chrono>

#include "../framework_define.hpp"

#define LOG_ERR  "[ERROR]"   // 标签 <错误>
#define LOG_WARN "[WARRING]" // 标签 <警告>
#define LOG_INFO "[INFO]"    // 标签 <信息>
#define LOG_TRC  "[TRACE]"   // 标签 <跟踪>
#define LOG_PERF "[PERF]"    // 标签 <性能>

#define SYSMD_LB_LOG std::string("[log_system]: ")

// format number => string, %xd(fill_zero).
std::string number_fmt_fillzero(uint32_t number, int32_t digits);

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
		std::vector<std::string> ExtractLogSegment(uint32_t Lines);
	}
	// get src time[sec].
	size_t GetTimeNow();
}

namespace LOGFILE {
	// async process. write folder.
	bool StartLogFileProcess(const char* folder);
	bool FreeLogFileProcess();
}

#endif