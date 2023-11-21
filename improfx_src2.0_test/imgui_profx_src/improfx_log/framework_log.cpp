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

vector<string> GLOBAL_LOG_CACHE    = {};
vector<string> GLOBAL_LOG_OFILETMP = {};

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
const std::unordered_map<std::string, std::string> HashLogLable = {
	{LOG_ERR,  "\033[31m"},
	{LOG_WARN, "\033[33m"},
	{LOG_INFO, "\033[90m"},
	{LOG_TRC,  "\033[96m"},
	{LOG_PERF, "\033[35m"}
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

	void LogInfoPush(const char* Lable, string LogStrText) {
		unique_lock<mutex> threadlock(LogMutex);

		if (LogStrText.find(LOG_WARN) != string::npos) { LogWarringLines++; };
		if (LogStrText.find(LOG_ERR) != string::npos)  { LogErrorLines++; };

		string pcslog = __get_timestamp() + ":" + Lable + ":" + LogStrText;
		GLOBAL_LOG_CACHE.push_back(pcslog);

		LogWriteQueue.push(pcslog);
		LogCondition.notify_one();

		auto find_color = HashLogLable.find(Lable);
		if (find_color != HashLogLable.end())
			cout << find_color->second << pcslog << " \033[0m" << endl;
	}

	Vector3T<size_t> LogLinesNumber() {
		Vector3T<size_t> return_value = {};
		{
			unique_lock<mutex> threadlock(LogMutex);

			return_value.vector_x = GLOBAL_LOG_CACHE.size(); // 总行数.
			return_value.vector_y = LogWarringLines;         // 警告行数.
			return_value.vector_z = LogErrorLines;           // 错误行数.
		}
		return return_value;
	}

	namespace ReadLogCache {
		vector<string> ExtractLogSegment(uint32_t Lines) {

			vector<string> return_str = {};
			return_str.clear();
			{
				size_t loglines =
					((int64_t)GLOBAL_LOG_CACHE.size() - Lines - 1) >= 0 ?
					((int64_t)GLOBAL_LOG_CACHE.size() - Lines - 1) : 0;

				unique_lock<mutex> threadlock(LogMutex);
				for (size_t i = loglines; i < GLOBAL_LOG_CACHE.size(); i++)
					return_str.push_back(GLOBAL_LOG_CACHE[i] + "\n");
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

#define LOGFILE_EXTENSION ".log"
namespace LOGFILE {

	thread* LogProcessThread = {};
	bool    LogProcessFlag   = true;

	void logprocess_write_file_eventloop(const char* folder) {
		// create name: folder + name(time) + ext.
		string filename = 
			folder + 
			to_string(chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now().time_since_epoch()).count()) +
			LOGFILE_EXTENSION;

		fstream logwrite(filename, ios::out | ios::app);
		LOGCONS::LogInfoPush(LOG_INFO, SYSMD_LB_LOG + "create log_file: " + filename);
		
		while (LogProcessFlag) {
			unique_lock<mutex> lock(LogMutex);
			LogCondition.wait(lock, [] { return !LogWriteQueue.empty() || !LogProcessFlag; });

			while (!LogWriteQueue.empty()) {
				const string& temp_logmsg = LogWriteQueue.front();

				logwrite << temp_logmsg << endl;
				LogWriteQueue.pop();
			}
		}
		logwrite.close();
	}

	bool StartLogFileProcess(const char* folder) {
		bool returnflag = true;

		if (filesystem::exists(folder) && filesystem::is_directory(folder)) {
			// init thread.
			try {
				LogProcessThread = new thread(logprocess_write_file_eventloop, folder);
				LOGCONS::LogInfoPush(LOG_INFO, SYSMD_LB_LOG + "start_thread success.");
			}
			catch (const exception& err) {
				LOGCONS::LogInfoPush(LOG_ERR, SYSMD_LB_LOG + "start_thread error: " + err.what());
				returnflag = false;
			}
		}
		else {
			LOGCONS::LogInfoPush(LOG_ERR, SYSMD_LB_LOG + "invalid folder: " + folder);
			returnflag = false;
		}
		return returnflag;
	}

	bool FreeLogFileProcess() {
		bool returnflag = true;
		try {
			LogProcessFlag = false;
			LogCondition.notify_one();

			LogProcessThread->join();
			LOGCONS::LogInfoPush(LOG_INFO, SYSMD_LB_LOG + "free_thread success.");
		}
		catch (const exception& err) {
			LOGCONS::LogInfoPush(LOG_ERR, SYSMD_LB_LOG + "free_thread error: " + err.what());
			returnflag = false;
		}
		return returnflag;
	}
}