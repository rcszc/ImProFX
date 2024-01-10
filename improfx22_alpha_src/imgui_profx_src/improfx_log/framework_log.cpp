// framework_log.
#include <fstream>
#include <thread>
#include <future>
#include <queue>
#include <condition_variable>
#include <filesystem>

#include "framework_log.hpp"

using namespace std;

mutex LogMutex = {};
vector<LOGCONS::ReadLogCache::LogCache> GLOBAL_LOG_CACHE = {};

size_t LogWarringLines = NULL, LogErrorLines = NULL;

queue<string> LogWriteQueue     = {};
condition_variable LogCondition = {};

#include <iomanip>
// time [xxxx.xx.xx.xx:xx:xx:xx ms].
string __get_timestamp() {
	auto timenow = chrono::system_clock::now();
	auto timemillisecond = chrono::duration_cast<chrono::milliseconds>(timenow.time_since_epoch()) % 1000;
	auto t = chrono::system_clock::to_time_t(timenow);
	tm _time = {};
#ifdef _WIN32
	localtime_s(&_time, &t);
#else
	localtime_r(&t, &_time);
#endif
	stringstream _sstream;
	_sstream << put_time(&_time, "[%Y.%m.%d %H:%M:%S") << " " << setfill('0') << setw(3) << timemillisecond.count() << " ms]";
	// windows / linux time stamp.
	return _sstream.str();
}

// global const hashlable.
const std::unordered_map<int32_t, std::string> HashLogLable = {
	{LOG_ERROR,   "\033[31m"},
	{LOG_WARNING, "\033[33m"},
	{LOG_INFO,    "\033[90m"},
	{LOG_TRACE,   "\033[96m"},
	{LOG_PERF,    "\033[35m"}
};

string number_fmt_fillzero(uint32_t number, int32_t digits) {
	// format: %xd.
	string result = to_string(number);
	// fill zero.
	while (result.length() < digits) {
		result = "0" + result;
	}
	return result;
}

namespace LOGCONS {
#include <io.h>
#include <fcntl.h>
	bool LOG_PRINT_SWITCH = true;

	void PushLogProcess(const LOGLABEL& Label, const string& LogStrText) {
		unique_lock<mutex> LogThreadLock(LogMutex);

		if (Label & LOG_WARNING) { LogWarringLines++; };
		if (Label & LOG_ERROR)   { LogErrorLines++; };

		const char* LogLabelTmp = "[NULL]";
		switch (Label) {
		case(LOG_ERROR):   { LogLabelTmp = "[ERROR]";   break; }
		case(LOG_WARNING): { LogLabelTmp = "[WARNING]"; break; }
		case(LOG_INFO):    { LogLabelTmp = "[INFO]";    break; }
		case(LOG_TRACE):   { LogLabelTmp = "[TRACE]";   break; }
		case(LOG_PERF):    { LogLabelTmp = "[PERF]";    break; }
		}

		string FmtLog = __get_timestamp() + ":" + LogLabelTmp + ":" + LogStrText;
		GLOBAL_LOG_CACHE.push_back(LOGCONS::ReadLogCache::LogCache(FmtLog, Label));

		LogWriteQueue.push(FmtLog);
		LogCondition.notify_one();

		auto FindLevelColor = HashLogLable.find(Label);
		if (FindLevelColor != HashLogLable.end() && LOG_PRINT_SWITCH)
			cout << FindLevelColor->second << FmtLog << " \033[0m" << endl;
	}

#define LOGGER_BUFFER_LEN 2048
	void PushLogger(const LOGLABEL& Label, const char* ModuleLabel, const char* LogText, ...) {
		char LoggerStrTemp[LOGGER_BUFFER_LEN] = {};

		va_list ParamArgs;
		va_start(ParamArgs, LogText);
		vsnprintf(LoggerStrTemp, LOGGER_BUFFER_LEN, LogText, ParamArgs);
		va_end(ParamArgs);

		PushLogProcess(Label, string(ModuleLabel) + LoggerStrTemp);
	}

	void SET_PRINTLOG_STATE(bool State) { LOG_PRINT_SWITCH = State; };

	Vector3T<size_t> LogLinesStatistics() {
		Vector3T<size_t> ReturnValue = {};
		{
			unique_lock<mutex> LogThreadLock(LogMutex);

			ReturnValue.vector_x = GLOBAL_LOG_CACHE.size(); // 总行数.
			ReturnValue.vector_y = LogWarringLines;         // 警告行数.
			ReturnValue.vector_z = LogErrorLines;           // 错误行数.
		}
		return ReturnValue;
	}

	namespace ReadLogCache {
		vector<LogCache> ExtractLogSegment(const uint32_t& Lines) {

			vector<LogCache> ReturnLogCache = {};
			ReturnLogCache.clear();
			{
				size_t RetLinesCount =
					((int64_t)GLOBAL_LOG_CACHE.size() - Lines - 1) >= 0 ?
					((int64_t)GLOBAL_LOG_CACHE.size() - Lines - 1) : 0;

				unique_lock<mutex> LogThreadLock(LogMutex);
				for (size_t i = RetLinesCount; i < GLOBAL_LOG_CACHE.size(); ++i)
					ReturnLogCache.push_back(GLOBAL_LOG_CACHE[i]);
			}
			return ReturnLogCache;
		}
	}

	size_t GetTimeNow() {
		return (size_t)chrono::duration_cast
			<chrono::microseconds>(
				chrono::steady_clock::now().time_since_epoch()
			).count();
	}
}

#define LOGFILE_EXTENSION ".log"
namespace LOGFILE {

	thread* LogProcessThread = {};
	bool    LogProcessFlag   = true;

	void logprocess_write_file_eventloop(const char* folder) {
		// create name: folder + name(time) + ext.
		string LogFileName = 
			folder + 
			to_string(chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now().time_since_epoch()).count()) +
			LOGFILE_EXTENSION;

		fstream WriteLog(LogFileName, ios::out | ios::app);
		LOGCONS::PushLogger(LOG_INFO, SYSMD_LB_LOG, "create log_file: %s", LogFileName.c_str());
		
		while (LogProcessFlag) {
			unique_lock<mutex> LogThreadLock(LogMutex);
			LogCondition.wait(LogThreadLock, [] { return !LogWriteQueue.empty() || !LogProcessFlag; });

			while (!LogWriteQueue.empty()) {
				const string& LogMsgTemp = LogWriteQueue.front();

				WriteLog << LogMsgTemp << endl;
				LogWriteQueue.pop();
			}
		}
		WriteLog.close();
	}

	bool StartLogFileProcess(const char* folder) {
		bool ReturnFlag = true;

		if (filesystem::exists(folder) && filesystem::is_directory(folder)) {
			// init thread.
			try {
				LogProcessThread = new thread(logprocess_write_file_eventloop, folder);
				LOGCONS::PushLogger(LOG_INFO, SYSMD_LB_LOG, "start thread success.");
			}
			catch (const exception& err) {
				LOGCONS::PushLogger(LOG_ERROR, SYSMD_LB_LOG, "start thread error: %s", err.what());
				ReturnFlag = false;
			}
		}
		else {
			LOGCONS::PushLogger(LOG_ERROR, SYSMD_LB_LOG, "invalid folder: %s", folder);
			ReturnFlag = false;
		}
		return ReturnFlag;
	}

	bool FreeLogFileProcess() {
		bool ReturnFlag = true;
		try {
			LogProcessFlag = false;
			LogCondition.notify_one();

			LogProcessThread->join();
			LOGCONS::PushLogger(LOG_INFO, SYSMD_LB_LOG, "free thread success.");
		}
		catch (const exception& err) {
			LOGCONS::PushLogger(LOG_ERROR, SYSMD_LB_LOG, "free thread error: %s", err.what());
			ReturnFlag = false;
		}
		return ReturnFlag;
	}
}

size_t     IMPROFX_GEN_NUMBER::TimeCountBuffer      = {};
std::mutex IMPROFX_GEN_NUMBER::TimeCountBufferMutex = {};