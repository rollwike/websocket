#ifndef __WEB_SOCKET_REQUEST_H__
#define __WEB_SOCKET_REQUEST_H__

#include <stdint.h>
#include <arpa/inet.h>
#include "log.h"

class CRequest
{
public:
	CRequest();

	~CRequest();

	int FectchWebSocketInfo(const char* msg);

	void Print();

	void Reset();

	char* getPayLoad();

	uint8_t getOpCode();
private:
	int FetchFin(const char* msg, int& pos);

	int FetchOpCode(const char* msg, int& pos);

	int FetchMask(const char* msg, int& pos);

	int FetchMaskingKey(const char* msg, int& pos);

	int FetchPayLoadLen(const char* msg, int& pos);

	int FetchPayLoad(const char* msg, int& pos);
private:
	uint8_t m_fin;
	uint8_t m_opcode;
	uint8_t m_mask;
	uint8_t m_maskkey[4];
	uint64_t m_payloadlen;
	char m_payload[2048];
};

#endif