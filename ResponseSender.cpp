#include "ResponseSender.hpp"

ResponseSender::ResponseSender()
{
}

void ResponseSender::Set(Config &config, int kq)
{
	m_config = config;
	m_kq = kq;
}

ResponseSender::~ResponseSender()
{
}

void ResponseSender::SendResponses(Document &document) // 다양한 에러처리 생각해주기
{
	std::vector<Response> &responses = document.GetResponse();
	for (std::vector<Response>::iterator it = responses.begin(); it != responses.end(); it++)
	{
		Response response = *it;
		std::cout << "response status is " << response.GetStatusCode() << std::endl;
		int clientSock = response.GetOriginFd();
		std::string responseStr = response.GetResponse();
		int len = write(clientSock, responseStr.c_str(), responseStr.length());
		if (len == -1 || static_cast<unsigned long>(len) != responseStr.length())
		{
			std::cout << "write error" << std::endl;
		}
		// write(clientSock, responseStr.c_str(), responseStr.length());
		// 헤더를 보고 서버연결 닫을지 말지 결정
		// 안닫으면 이벤트 writeable 빼주기
		close(clientSock);
	}
	responses.clear();
	// 이것도 무작정 clear해주면 안됌
	// 정적요청을 무조건 완벽하게 처리하고 ㅇ
}