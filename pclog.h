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
#include<Windows.h>
#else
#include <sys/time.h>
#endif

#if defined(ANDROID) && !defined(PCLOG_ANDROID_USE_STDOUT)
	#include <android/log.h>
	#define PCLOG_ANDROID
#endif

#define LOG(level) if (level > pclog::Log::ReportingLevel()) ; else pclog::Log(level).get()
#define LOG_E  LOG(logERROR)
#define LOG_I  LOG(logINFO)
#define LOG_W  LOG(logWARNING)
#define LOG_D  LOG(logDEBUG)

#include "to_string.h"

enum PCLogLevel {
    logERROR, logWARNING, logINFO, logDEBUG,
    logDEBUG1, logDEBUG2, logDEBUG3, logDEBUG4
};

namespace pclog {

    inline std::mutex &mutex() {
        static std::mutex mutex;
        return mutex;
    }

#ifdef _WIN32
    inline std::string getErrorString2(int err = 0)
    {
        err = err ? err : WSAGetLastError();

        LPVOID lpMsgBuf;
        DWORD dw = GetLastError();

        FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL
        );


        std::string msg((LPTSTR)lpMsgBuf);
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
#ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO scbi;
#elif defined(PCLOG_ANDROID)
        std::ostringstream os;
#endif
        inline Log(PCLogLevel level = logINFO) : lv(level) {
            mutex().lock(); // TODO: this is very slow, should use buffer
        }

        ~Log() {
#if !defined(_WIN32) && !defined(PCLOG_ANDROID)
            if (lv == logERROR || lv == logWARNING)
                std::cerr << "\e[0m ";
#endif

#ifdef PCLOG_ANDROID
            int ap = ANDROID_LOG_INFO;
            switch (lv) {
                case logERROR:
                    ap = ANDROID_LOG_ERROR;
                    break;
                case logWARNING:
                    ap = ANDROID_LOG_WARN;
                    break;
                case logDEBUG:
                    ap = ANDROID_LOG_DEBUG;
                    break;
                default:
                    ap = ANDROID_LOG_INFO;
                    break;
            }
            __android_log_print(ap,
                                "pclog",
                                "%s",
                                os.str().c_str());
#else
            if (lv == logERROR || lv == logWARNING)
                std::cerr << std::endl << std::flush;
            else
            std::cout << std::endl << std::flush;
#endif

#ifdef _WIN32
            if (lv == logERROR || lv == logWARNING)
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), scbi.wAttributes);
#endif
            mutex().unlock();
        }

        inline std::ostream &get() {

#ifdef PCLOG_ANDROID
            std::ostream &ls(this->os);
#else
            std::ostream& ls((lv == logERROR || lv == logWARNING) ? std::cerr : std::cout);
#endif

            std::string cl;

#ifdef _WIN32
            if (lv == logERROR || lv == logWARNING) {
                GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &scbi);
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), ((lv == logWARNING) ? (FOREGROUND_GREEN|FOREGROUND_RED) : FOREGROUND_RED) | FOREGROUND_INTENSITY);
            }
#elif !defined(PCLOG_ANDROID)
            if (lv == logERROR)
                cl = "\e[91m";
            else if (lv == logWARNING)
                cl = "\e[93m";
#endif
            ls << (cl + std::string(lv > logDEBUG ? (lv - logDEBUG) * 2 : 0, ' '));
            return ls;
        }

        inline static PCLogLevel ReportingLevel() {
            return logDEBUG3; // logDebugHttp;
        }
    };
}