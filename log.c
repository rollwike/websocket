#include "log.h"

void DEBUG_LOG(const char* msg, ...)
{
	char message[256] = {0};
	va_list vars;
	va_start(vars, msg);
	vsprintf(message, msg, vars);
	va_end(vars);
	CLOGMgr.WriteLog(message);
}

CLog *CLog::m_log = NULL;

CLog::CLog() : 
	m_time(0), 
	m_fd(NULL), 
	m_filepath(), 
	m_msg(),
	m_t(NULL), 
	m_lastLogTime()
{
	CreateLogFile();
}

CLog::~CLog()
{
	if (m_fd > 0)
	{
		fclose(m_fd);
		m_fd = NULL;
	}
}

void CLog::CreateLogFile()
{
	if (m_fd)
	{
		fclose(m_fd);
		m_fd = NULL;
	}

	sprintf(m_filepath, "./log/log_");
	time(&m_time);
	m_t = localtime(&m_time);

	sprintf(m_filepath + 10, "%02d_%02d", m_t->tm_mon + 1, m_t->tm_mday);
	m_fd = fopen(m_filepath, "a+");
}

CLog& CLog::log()
{
	if (NULL == m_log)
	{
		m_log = new CLog();
	}
	return *m_log;
}

void CLog::WriteLog(const char* msg)
{
	time(&m_time);
	m_t = localtime(&m_time);
	sprintf(m_msg, "[%02d_%02d_%02d] %s \n", m_t->tm_hour, m_t->tm_min, m_t->tm_sec, msg);
	fwrite(m_msg, strlen(m_msg), 1, m_fd);
	fflush(m_fd);
}