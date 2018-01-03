#pragma once

/*
An efficient logging class that works on Linux, Windows and Android
Usage:
LOG(logERROR) << "whoops";
LOG(logWARNING) << "awww";
LOG(logINFO) << "aha";
LOG(logDEBUG) << "h4x0r";
*/

#include <string>
#include <string.h> // required by strerror_r
#include <sstream>
#include <iostream>
#include <locale>
#include <mutex>

#ifdef _WIN32
#include<windows.h>
#else
#include <sys/time.h>
#endif

#if defined(ANDROID) && !defined(PCLOG_ANDROID_USE_STDOUT)
#include <android/log.h>
#define PCLOG_ANDROID
#endif

#define PCLOG_USE_BUFFER

#define LOG(level) if (level > pclog::Log::ReportingLevel()) ; else pclog::Log(level).get()
#define LOG_E  LOG(logERROR)
#define LOG_I  LOG(logINFO)
#define LOG_W  LOG(logWARNING)
#define LOG_D  LOG(logDEBUG)
#define LOG_V  LOG(logVERBOSE)

#include "to_string.h"

enum PCLogLevel {
	logERROR, logWARNING, logINFO, logDEBUG,
	logDEBUG1, logDEBUG2, logDEBUG3, logDEBUG4, logVERBOSE
};

namespace pclog {

	inline std::mutex &mutex() {
		static std::mutex mutex;
		return mutex;
	}

#ifdef _WIN32

	inline std::string getErrorString2(int err = 0) {
//		err = err ? err : WSAGetLastError();

		LPVOID lpMsgBuf;
		DWORD dw = GetLastError();

		FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf, 0, NULL
		);


		std::string msg((LPTSTR) lpMsgBuf);
		LocalFree(lpMsgBuf);

		msg += " (" + std::to_string(dw) + ")";

		return msg;
	}

#endif

	inline std::string getErrorString(int rc) {
		char errmsg[1024];
#ifdef _WIN32
		::strerror_s(errmsg, sizeof errmsg, rc);
#else
		strerror_r(rc, errmsg, sizeof errmsg);
#endif
		return errmsg;
	}


	class Log {
	public:
		const PCLogLevel lv;
#ifdef PCLOG_USE_BUFFER
		std::ostringstream buf;
#endif

#ifdef _WIN32
		CONSOLE_SCREEN_BUFFER_INFO scbi;
#endif

		inline Log(PCLogLevel level = logINFO) : lv(level) {

		}

#ifdef ANDROID
		inline int androidLogLevel() {
			switch (lv) {
				case logERROR:
					return ANDROID_LOG_ERROR;
				case logWARNING:
					return ANDROID_LOG_WARN;
				case logINFO:
					return ANDROID_LOG_INFO;
				case logDEBUG:
					return ANDROID_LOG_DEBUG;
				case logVERBOSE:
					return ANDROID_LOG_VERBOSE;
				default:
					return ANDROID_LOG_DEFAULT;
			}
		}
#endif

		~Log() {
			mutex().lock();
			std::ostream &os((lv == logERROR || lv == logWARNING) ? std::cerr : std::cout);

#ifdef _WIN32
			if (lv == logERROR || lv == logWARNING) {
				GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &scbi);
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
										((lv == logWARNING) ? (FOREGROUND_GREEN | FOREGROUND_RED) : FOREGROUND_RED) |
										FOREGROUND_INTENSITY);
			}
#elif !defined(PCLOG_ANDROID)
			if (lv == logERROR)
				os << "\e[91m";
			else if (lv == logWARNING)
				os <<  "\e[93m";
#endif

#ifdef PCLOG_ANDROID
			__android_log_print(androidLogLevel(), "pclog",  "%s", buf.str().c_str());
#else
			os << buf.str();
			if (lv == logERROR || lv == logWARNING) {
#ifdef _WIN32
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), scbi.wAttributes);
#else
				os << "\e[0m ";
#endif
			}
			os << std::endl << std::flush;
			buf.clear();
#endif
			mutex().unlock();
		}

		inline std::ostream &get() {
			// add space prefix based on debug level
			buf << std::string(lv > logDEBUG ? (lv - logDEBUG) * 2 : 0, ' ');
			return buf;
		}

		inline static PCLogLevel ReportingLevel() {
			return logDEBUG3; // logDebugHttp;
		}
	};
}