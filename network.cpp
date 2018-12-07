#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include "log.h"
#include "network.h"


CNetWork *CNetWork::m_network = NULL;

CNetWork::CNetWork() :
	m_epollfd(0),
	m_listenfd(0),
	m_handlerMap()
{
	if (0 != init())
	{
		exit(1);
	}
}

CNetWork::~CNetWork()
{

}

int CNetWork::init()
{
	m_listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == m_listenfd)
	{
		DEBUG_LOG("创建套接字失败");
		return -1;
	}

	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(PORT);
	if (-1 == bind(m_listenfd, (struct sockaddr*)(&server_addr), sizeof(server_addr)))
	{
		DEBUG_LOG("绑定套接字失败");
		return -1;
	}

	if (-1 == listen(m_listenfd, 5))
	{
		DEBUG_LOG("监听套接字失败");
		return -1;
	}

	m_epollfd = epoll_create(MAXEPOLLSIZE);

	ctl_event(m_listenfd, true);
	DEBUG_LOG("服务器启动成功");
	return 0;
}

int CNetWork::epoll_loop()
{
	struct sockaddr_in clientaddr;
	socklen_t clientlen;
	int nfds = 0;
	int fd = 0;
	int buflen = 0;
	struct epoll_event event[MAXEPOLLSIZE];
	while(true)
	{
		nfds = epoll_wait(m_epollfd, event, MAXEPOLLSIZE, TIME_WAIT);
		for (int i = 0; i < nfds; ++i)
		{
			if (m_listenfd == event[i].data.fd)
			{
				fd = accept(m_listenfd, (struct sockaddr*)(&clientlen), &clientlen);
				ctl_event(fd, true);
			}
			else if (event[i].events & EPOLLIN)
			{
				if ((fd = event[i].data.fd) <= 0)
				{
					continue;
				}
				
				CHandler *handler = m_handlerMap[fd];
				if (NULL == handler)
				{
					continue;
				}

				if ((buflen = read(fd, handler->getBuf(), BUF_LEN)) <= 0)
				{
					ctl_event(fd, false);
				}
				else
				{
					handler->process(buflen);
				}
			}
		}
	}

	return 0;
}

int CNetWork::setNoBlock(int fd)
{
	int flags = 0;
	if (-1 == (flags = fcntl(fd, F_GETFL, 0)))
	{
		flags = 0;
	}

	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void CNetWork::ctl_event(int fd, bool flag)
{
	struct epoll_event event;
	event.data.fd = fd;
	event.events = flag ? EPOLLIN : 0;
	epoll_ctl(m_epollfd, flag ? EPOLL_CTL_ADD : EPOLL_CTL_DEL, fd,  &event);
	if (flag)
	{
		setNoBlock(fd);
		m_handlerMap[fd] = new CHandler(fd);
		if (fd != m_listenfd)
		{
			DEBUG_LOG("fd: %d 加入epoll循环", fd);
		}
	}
	else
	{
		close(fd);
		delete m_handlerMap[fd];
		m_handlerMap.erase(fd);
		DEBUG_LOG("fd: %d 退出epoll循环", fd);
	}
}

void CNetWork::run()
{
	epoll_loop();
}

CNetWork& CNetWork::getInstance()
{
	if (NULL == m_network)
	{
		m_network = new CNetWork();
	}

	return *m_network;
}

