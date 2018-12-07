#ifndef __WEB_SOCKET_NETWORK_H__
#define __WEB_SOCKET_NETWORK_H__

#include "handler.h"

#define PORT 9000
#define TIME_WAIT 100
#define BUF_LEN 2048
#define MAXEPOLLSIZE 20

typedef std::map<int , CHandler*> WEB_SOCKET_HANDLER_MAP;

class CNetWork
{
private:
	CNetWork();

	~CNetWork();

	int init();

	int epoll_loop();

	int setNoBlock(int fd);

	void ctl_event(int fd, bool flag);
public:
	void run();

	static CNetWork& getInstance();

private:
	int m_epollfd;
	int m_listenfd;
	WEB_SOCKET_HANDLER_MAP m_handlerMap;
	static CNetWork* m_network;
};

#define CNetWorkMgr CNetWork::getInstance()

#endif