#ifndef __WEB_SOCKET_RESPONSE_H__
#define __WEB_SOCKET_RESPONSE_H__

#include <string.h>
#include <stdint.h>
#include <sstream>
#include <arpa/inet.h>
#include "log.h"

class CResponse
{
public:
	CResponse();

	~CResponse();

	int SendData(std::string str, int frameType, char* outFrame);
};

#endif