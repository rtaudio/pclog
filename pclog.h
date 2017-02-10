#pragma once

#ifndef _PCLOG_HEADER_
#define _PCLOG_HEADER_

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

#ifdef _WIN32
#include<Windows.h>
#else
#include <sys/time.h>
#endif

#define LOG(level) if (level > pclog::Log::ReportingLevel()) ; else pclog::Log(level).get()
	
enum PCLogLevel {
	logERROR, logWARNING, logINFO, logDEBUG,
	logDEBUG1, logDEBUG2, logDEBUG3, logDEBUG4
};

namespace pclog {


	inline std::string getErrorString(int rc) {
		char errmsg[1024];
#ifdef _WIN32
		::strerror_s(errmsg, rc);
#else
		strerror_r(rc, errmsg, sizeof errmsg);
#endif
		return errmsg;
	}



#ifdef ANDROID
#include <android/log.h>
#endif

class Log
{
public:
	const PCLogLevel lv;
#ifdef _WIN32
	CONSOLE_SCREEN_BUFFER_INFO scbi;
#endif

#ifdef ANDROID
	std::ostringstream os;
#endif

	inline Log(PCLogLevel level = logINFO) : lv(level) {	}
	
	~Log() {
#if !defined(_WIN32) && !defined(ANDROID)
		if (lv == logERROR || lv == logWARNING)
			std::cout << "\e[0m ";
#endif

#ifdef ANDROID
		int ap = ANDROID_LOG_INFO;
		switch (lv) {
		case logERROR: ap = ANDROID_LOG_ERROR; break;
		case logWARNING: ap = ANDROID_LOG_WARN; break;
		}
		__android_log_print(ap,
			"ulltra",
			"%s",
			os.str().c_str());
#else
		std::cout << std::endl << std::flush;
#endif
		
#ifdef _WIN32
		if (lv == logERROR || lv == logWARNING)
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), scbi.wAttributes);
#endif
	}
	inline std::ostream& get() {

#ifdef ANDROID
		std::ostream& ls(this->os);
#else
		std::ostream& ls(std::cout);
#endif

		std::string cl;

#ifdef _WIN32
		if (lv == logERROR || lv == logWARNING) {
			GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &scbi);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), ((lv == logWARNING) ? (FOREGROUND_GREEN|FOREGROUND_RED) : FOREGROUND_RED) | FOREGROUND_INTENSITY);
		}
#elif !defined(ANDROID)
		if (lv == logERROR)
			cl = "\e[91m";
		else if (lv == logWARNING)
			cl = "\e[93m";
#endif
		ls << (cl + std::string(lv > logDEBUG ? (lv - logDEBUG)*2 : 0, ' '));
		return ls;
	}

	inline static PCLogLevel ReportingLevel() {
		return logDEBUG3; // logDebugHttp;
	}
};


	
}

#endif