#include <unistd.h>
#include "handler.h"

CHandler::CHandler(int fd) :
	m_buf(),
	m_headMap(),
	m_status(WEBSOCKET_STATUS_UNCONNECT),
	m_fd(fd),
	m_request(new CRequest),
	m_response(new CResponse)
{

}

CHandler::~CHandler()
{
	if (NULL != m_request)
	{
		delete m_request;
		m_request = NULL;
	}

	if (NULL != m_response)
	{
		delete m_response;
		m_response = NULL;
	}
}

int CHandler::process(int len)
{
	if (WEBSOCKET_STATUS_UNCONNECT == m_status)
	{
		return handshark();
	}

	m_request->FectchWebSocketInfo(m_buf);
	if (WS_CLOSING_FRAME != m_request->getOpCode())
	{
		replyClient(m_request->getPayLoad());
	}
	
	m_request->Print();
	memset(m_buf, 0, sizeof(m_buf));
	return 0;
}

int CHandler::handshark()
{
	char reqMsg[1024] = {0};
	m_status = WEBSOCKET_STATUS_HANDLER;
	FetchHttpInfo();
	parseStr(reqMsg);
	memset(m_buf, 0, sizeof(m_buf));
	return sendData(reqMsg);
}

int CHandler::FetchHttpInfo()
{
	std::istringstream s(m_buf);
	std::string request;

	std::getline(s, request);
	if (request[request.size()-1] == '\r') {
		request.erase(request.end()-1);
	} else {
		return -1;
	}

	std::string header;
	std::string::size_type end;

	while (std::getline(s, header) && header != "\r") {
		if (header[header.size()-1] != '\r') {
			continue; //end
		} else {
			header.erase(header.end()-1);	//remove last char
		}
		end = header.find(": ",0);
		if (end != std::string::npos) {
			std::string key = header.substr(0,end);
			std::string value = header.substr(end+2);
			m_headMap[key] = value;
		}
	}

	return 0;
}

void CHandler::parseStr(char* request)
{
	strcat(request, "HTTP/1.1 101 Switching Protocols\r\n");
	strcat(request, "Connection: upgrade\r\n");
	strcat(request, "Sec-WebSocket-Accept: ");
	std::string server_key = m_headMap["Sec-WebSocket-Key"];
	server_key += MAGIC_KEY;

	SHA1 sha;
	unsigned int message_digest[5];
	sha.Reset();
	sha << server_key.c_str();

	sha.Result(message_digest);
	for (int i = 0; i < 5; i++) {
		message_digest[i] = htonl(message_digest[i]);
	}
	server_key = base64_encode(reinterpret_cast<const unsigned char*>(message_digest),20);
	server_key += "\r\n";
	strcat(request, server_key.c_str());
	strcat(request, "Upgrade: websocket\r\n\r\n");
}

int CHandler::sendData(const char* msg)
{
	return write(m_fd, msg, strlen(msg));
}

int CHandler::replyClient(char* msg)
{
	std::string replyStr(msg);
	char outFrame[2048] = {0};
	int len = m_response->SendData(msg, WS_TEXT_FRAME, outFrame);
	sendData(outFrame);
	return 0;
}