#include "response.h"

CResponse::CResponse()
{

}

CResponse::~CResponse()
{

}

int CResponse::SendData(std::string str, int frameType, char* outFrame)
{
	uint32_t len = str.size();

	uint8_t payLoadFiledExtraBytes = (len <= 0x7d) ? 0 : 2;
	uint8_t headSize = 2 + payLoadFiledExtraBytes;
	uint8_t* frameHeader = new uint8_t[headSize];
	memset(frameHeader, 0, sizeof(frameHeader));
	frameHeader[0] = (uint8_t)(0x80 | frameType);
	if (len <= 0x7d)
	{
		frameHeader[1] = (uint8_t)(len);
	}
	else
	{
		frameHeader[1] = 0x7e;
		uint16_t datalen = htons(len);
		memcpy(&frameHeader[2], &datalen, payLoadFiledExtraBytes);
	}

	uint32_t frameSize = headSize + len;
	memcpy(outFrame, frameHeader, headSize);
	memcpy(outFrame + headSize, str.c_str(), len);

	delete[] frameHeader;
	return frameSize;
}