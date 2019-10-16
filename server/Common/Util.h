#ifndef UTIL
#define UTIL

#include "../Predef.h"
#include "Sha256.h"
#include "md5.h"

namespace UTIL
{
	static void	Sleep(unsigned int dwMilliseconds)
	{
#ifdef OS_WINDOWS
		::Sleep(dwMilliseconds);
#else
		struct timespec req;
		req.tv_sec = 0;
		req.tv_nsec = dwMilliseconds * 1000000;
		nanosleep(&req, NULL);
#endif
		return;
	}

	static tm GetCurrTmTime()
	{
		time_t rawtime;
		struct tm* timeinfo;

		time(&rawtime);
		timeinfo = localtime(&rawtime);

		return *timeinfo;
	}

	static std::string GetSessionKey(std::string seed)
	{
		std::string sha2 = sha256(seed);	// 64

		tm ctime = UTIL::GetCurrTmTime();
		char buff[64];
		sprintf(buff, "%d-%d-%d-%d-%d-%d", ctime.tm_year, ctime.tm_mon, ctime.tm_mday, ctime.tm_hour, ctime.tm_min, ctime.tm_sec);
		std::string md = md5(std::string(buff));	// 32

		return sha2 + md;	// 64 + 32 = 96
	}

	static void Log(const std::string& fmt)
	{
		Application::instance().logger().information(fmt);
	}

	static void Log(const std::string& fmt, const Poco::Any& value1)
	{
		Application::instance().logger().information(fmt, value1);
	}

	static void Log(const std::string& fmt, const Poco::Any& value1, const Poco::Any& value2)
	{
		Application::instance().logger().information(fmt, value1, value2);
	}

	static void Log(const std::string& fmt, const Poco::Any& value1, const Poco::Any& value2, const Poco::Any& value3)
	{
		Application::instance().logger().information(fmt, value1, value2, value3);
	}

	static void Log(const std::string& fmt, const Poco::Any& value1, const Poco::Any& value2, const Poco::Any& value3, const Poco::Any& value4)
	{
		Application::instance().logger().information(fmt, value1, value2, value3, value4);
	}
}

#endif