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
		// std::cout << "response status is " << response.GetStatusCode() << std::endl;
		int clientSock = response.GetOriginFd();
		std::string responseStr = response.GetResponse();
		std::cout << std::endl << responseStr << std::endl;
		int len = write(clientSock, responseStr.c_str(), responseStr.length());
		// if (len == -1 || static_cast<unsigned long>(len) != responseStr.length())
		if (len == -1)
		{
			std::cout << "write response error in fd" << clientSock << std::endl;
			// std::cout << "response string is " << responseStr << std::endl;
			// std::cout << "end write error\n\n";
			std::cout << "errno is " << errno << std::endl;
		}
		// write(clientSock, responseStr.c_str(), responseStr.length());
		// 헤더를 보고 서버연결 닫을지 말지 결정
		// 안닫으면 이벤트 writeable 빼주기
		if (response.GetHeader("Connection") == "close")
		{
			if (close(clientSock) < 0)
			{
				std::cout << "close error in connection close fd " << clientSock << "\n";
				// throw std::runtime_error("in sender connection close close error");
			}
			document.GetFdEvent().erase(clientSock);
			// std::cout << "Connection closed fd " << clientSock << "\n";
		}
		else
		{
			struct kevent ev;
			EV_SET(&ev, clientSock, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
			kevent(m_kq, &ev, 1, NULL, 0, NULL);
		}
		// close(clientSock);
	}
	responses.clear();
	// 이것도 무작정 clear해주면 안됌
	// 정적요청을 무조건 완벽하게 처리하고 ㅇ
}