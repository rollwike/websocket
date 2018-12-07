#include "request.h"

CRequest::CRequest() :
	m_fin(0),
	m_opcode(0),
	m_mask(0),
	m_maskkey(),
	m_payloadlen(0),
	m_payload()
{

}

CRequest::~CRequest()
{

}

int CRequest::FectchWebSocketInfo(const char* msg)
{
	int pos = 0;
	FetchFin(msg, pos);
	FetchOpCode(msg, pos);
	FetchMask(msg, pos);
	FetchPayLoadLen(msg, pos);
	FetchMaskingKey(msg, pos);
	return FetchPayLoad(msg, pos);
}

void CRequest::Print()
{
	DEBUG_LOG("WEBSOCKET PROTOCOL\n"
				"FIN: %d\n"
				"OPCODE: %d\n"
				"MASK: %d\n"
				"PAYLOADLEN: %d\n"
				"PAYLOAD: %s",m_fin, m_opcode, m_mask, m_payloadlen, m_payload);
	Reset();
}

void CRequest::Reset()
{
	m_fin = 0;
	m_opcode = 0;
	m_mask = 0;
	memset(m_maskkey, 0, sizeof(m_maskkey));
	m_payloadlen = 0;
	memset(m_payload, 0, sizeof(m_payload));
}

int CRequest::FetchFin(const char* msg, int& pos)
{
	m_fin = (unsigned char)msg[pos] >> 7;
	return 0;
}

int CRequest::FetchOpCode(const char* msg, int& pos)
{
	m_opcode = msg[pos] & 0x0f;
	pos++;
	return 0;
}

int CRequest::FetchMask(const char* msg, int& pos)
{
	m_mask = (unsigned char)msg[pos] >> 7;
	return 0;
}

int CRequest::FetchPayLoadLen(const char* msg, int& pos)
{
	m_payloadlen = msg[pos] & 0x7f;
	pos++;
	if (126 == m_payloadlen)
	{
		uint16_t len = 0;
		memcpy(&len, msg + pos, 2);
		pos += 2;
		m_payloadlen = ntohs(len);
	}
	else if (127 == m_payloadlen)
	{
		uint32_t len = 0;
		memcpy(&len, msg + pos, 4);
		pos += 4;
		m_payloadlen = ntohl(len);
	}

	return 0;
}

int CRequest::FetchMaskingKey(const char* msg, int& pos)
{
	if (1 != m_mask)
	{
		return 0;
	}

	for (int i = 0; i < 4; ++i)
	{
		m_maskkey[i] = msg[pos + i];
	}

	pos += 4;
	return 0;
}

int CRequest::FetchPayLoad(const char* msg, int& pos)
{
	memset(m_payload, 0, sizeof(m_payload));
	if (1 != m_mask)
	{
		memcpy(&m_payload, msg + pos, m_payloadlen);
	}
	else 
	{
		for (uint i = 0; i < m_payloadlen; ++i)
		{
			int j = i % 4;
			m_payload[i] = msg[pos + i] ^ m_maskkey[j];
		}
	}
	pos += m_payloadlen;
	return 0;
}

char* CRequest::getPayLoad()
{
	return m_payload;
}

uint8_t CRequest::getOpCode()
{
	return m_opcode;
}