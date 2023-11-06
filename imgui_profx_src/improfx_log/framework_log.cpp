// framework_log.
#include <fstream>
#include <thread>
#include <future>
#include <filesystem>

#include "framework_log.hpp"

using namespace std;

mutex _logpushmutex = {};

vector<string> GLOBAL_LOG_CACHE    = {};
vector<string> GLOBAL_LOG_OFILETMP = {};

size_t LogWarringLines = NULL, LogErrorLines = NULL;

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
const std::unordered_map<std::string, std::string> HashLogLable = {
	{LOG_ERR,  "\033[31m"},
	{LOG_WARN, "\033[33m"},
	{LOG_INFO, "\033[90m"},
	{LOG_TRC,  "\033[96m"},
	{LOG_PERF, "\033[35m"}
};

namespace LOGCONS {
#include <io.h>
#include <fcntl.h>

	void LogInfoPush(const char* Lable, string LogStrText) {
		unique_lock<mutex> threadlock(_logpushmutex);

		if (LogStrText.find(LOG_WARN) != string::npos) { LogWarringLines++; };
		if (LogStrText.find(LOG_ERR) != string::npos)  { LogErrorLines++; };

		GLOBAL_LOG_CACHE.push_back(LogStrText);

		auto FindLable = HashLogLable.find(Lable);
		if (FindLable != HashLogLable.end())
			cout << FindLable->second << __get_timestamp() + ":" + Lable + ":" << LogStrText << "\033[0m" << endl;
	}

	Vector3T<size_t> LogLinesNumber() {
		Vector3T<size_t> return_value = {};
		{
			unique_lock<mutex> threadlock(_logpushmutex);

			return_value.vector_x = GLOBAL_LOG_CACHE.size(); // 总行数.
			return_value.vector_y = LogWarringLines;         // 警告行数.
			return_value.vector_z = LogErrorLines;           // 错误行数.
		}
		return return_value;
	}

	namespace ReadLogCache {
		string ExtractLogSegment(uint32_t Lines) {

			static string return_str = {};
			return_str.clear();
			{
				size_t loglines = ((int64_t)GLOBAL_LOG_CACHE.size() - Lines - 1) >= 0 ?
					((int64_t)GLOBAL_LOG_CACHE.size() - Lines - 1) : 0;

				unique_lock<mutex> threadlock(_logpushmutex);
				for (size_t i = loglines; i < GLOBAL_LOG_CACHE.size(); i++)
					return_str += GLOBAL_LOG_CACHE[i] + "\n";
			}
			return return_str;
		}
	}

	size_t GetTimeNow() {
		return (size_t)chrono::duration_cast
			<chrono::seconds>(
				chrono::steady_clock::now().time_since_epoch()
			).count();
	}
}

#define LOGFILE_COUNT_FILE 5      // name = %5d
#define LOGFILE_EXTENSION  ".log"

namespace LOGFILE {

	thread* LogProcessThread = {};
	mutex   WriteMutex       = {};

	bool FLAG_EVENTLOOP   = true;
	bool FLAG_FAILED_FILE = false;

	string number_fmt_fill(uint32_t number, int32_t digits) {
		string result = to_string(number);

		while (result.length() < digits) {
			result = "0" + result;
		}
		return result;
	}

	void logprocess_write_file_eventloop(const char* folder, int64_t timer) {
		uint32_t count_number = NULL;
		chrono::steady_clock::time_point starttime = chrono::steady_clock::now();

		// log process eventloop.
		while (FLAG_EVENTLOOP) {

			int64_t elapsed = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - starttime).count();
			if (elapsed >= timer) {
				string logpath = folder + number_fmt_fill(count_number, LOGFILE_COUNT_FILE) + LOGFILE_EXTENSION;

				LOGCONS::LogInfoPush(LOG_INFO, SYSTEM_MODULE_LABLE_log + "log process... save file: " + logpath);
				{
					unique_lock<mutex> threadlock(_logpushmutex);
					// copy: log cache => write temp.
					GLOBAL_LOG_OFILETMP = GLOBAL_LOG_CACHE;
					GLOBAL_LOG_CACHE.clear();
				}
				ofstream writelog(logpath);
				if (writelog) {
					for (const auto& logstr : GLOBAL_LOG_OFILETMP)
						writelog << logstr << '\n';
				}
				else {
					unique_lock<mutex> threadlock(WriteMutex);
					FLAG_FAILED_FILE = true;
				}
				writelog.close();
				GLOBAL_LOG_OFILETMP.clear();

				++count_number;
				starttime = chrono::steady_clock::now();
			}
		}
	}

	bool LogFileStatus() {
		unique_lock<mutex> threadlock(WriteMutex);
		return FLAG_FAILED_FILE;
	}

	bool StartLogFileProcess(const char* folder, int64_t timer) {
		bool returnflag = true;

		if (filesystem::exists(folder) && filesystem::is_directory(folder)) {
			// init thread.
			try {
				LogProcessThread = new thread(logprocess_write_file_eventloop, folder, timer);
				LOGCONS::LogInfoPush(LOG_INFO, SYSTEM_MODULE_LABLE_log + "start_thread success.");
			}
			catch (const exception& err) {
				LOGCONS::LogInfoPush(LOG_ERR, SYSTEM_MODULE_LABLE_log + "start_thread error: " + err.what());
				returnflag = false;
			}
		}
		else {
			LOGCONS::LogInfoPush(LOG_ERR, SYSTEM_MODULE_LABLE_log + "invalid folder: " + folder);
			returnflag = false;
		}
		return returnflag;
	}

	bool FreeLogFileProcess() {
		bool returnflag = true;
		try {
			FLAG_EVENTLOOP = false;
			LogProcessThread->join();

			LOGCONS::LogInfoPush(LOG_INFO, SYSTEM_MODULE_LABLE_log + "free_thread success.");
		}
		catch (const exception& err) {
			LOGCONS::LogInfoPush(LOG_ERR, SYSTEM_MODULE_LABLE_log + "free_thread error: " + err.what());
			returnflag = false;
		}
		return returnflag;
	}
}