#ifndef __WEB_SOCKET_HANDLER_H__
#define __WEB_SOCKET_HANDLER_H__

#include <arpa/inet.h>
#include <map>
#include <iostream>
#include <sstream>
#include "request.h"
#include "response.h"
#include "log.h"
#include "sha1.h"
#include "base64.h"


#define MAGIC_KEY "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

enum WEBSOCKET_STATUS
{
	WEBSOCKET_STATUS_UNCONNECT 	= 0,
	WEBSOCKET_STATUS_HANDLER	= 1
};

enum WS_FrameType
{
    WS_EMPTY_FRAME = 0xF0,
    WS_ERROR_FRAME = 0xF1,
    WS_TEXT_FRAME   = 0x01,
    WS_BINARY_FRAME = 0x02,
    WS_PING_FRAME = 0x09,
    WS_PONG_FRAME = 0x0A,
    WS_OPENING_FRAME = 0xF3,
    WS_CLOSING_FRAME = 0x08
};

typedef std::map<std::string, std::string> HEADER_MAP;

class CHandler
{
public:
	CHandler(int fd);

	~CHandler();

	int process(int len = 0);

	inline char* getBuf();
private:
	int handshark();

	void parseStr(char* request);

	int FetchHttpInfo();

	int sendData(const char* msg);

	int replyClient(char* msg);
private:
	char m_buf[2048];
	HEADER_MAP m_headMap;
	WEBSOCKET_STATUS m_status;
	int m_fd;
	CRequest *m_request;
	CResponse *m_response;

};

inline char* CHandler::getBuf()
{
	return m_buf;
}

#endif