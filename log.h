#ifndef __WEB_SOCKET_LOG_H__
#define __WEB_SOCKET_LOG_H__

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>

void DEBUG_LOG(const char* msg, ...);

class CLog
{
private:
	CLog();

	~CLog();

	void CreateLogFile();
public:
	static CLog& log();

	void WriteLog(const char* msg);
private:
	static CLog *m_log;
	time_t m_time;
	FILE *m_fd;
	char m_filepath[32];
	char m_msg[256];
	struct tm *m_t;
	struct tm m_lastLogTime;
};

#define CLOGMgr CLog::log()

#endif