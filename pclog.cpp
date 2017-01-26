#include "logging.h"

namespace pclog {

	Log::~Log() {
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

}