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

void ResponseSender::SendResponses(Document &document, int fd) // 다양한 에러처리 생각해주기
{
	ResponseInfo* info = document.GetResponse(fd);
	Response& response = info->response;
	struct kevent ev;
	// std::cout << "response status is " << response.GetStatusCode() << std::endl;
	int len;
	int& start = info->start;
	std::string& responseStr = response.GetResponse();
	// std::cout << "responseStr is " << responseStr << std::endl;
	const char *buf = responseStr.c_str();
	int& rest = info->rest;
	// std::cout << "start is " << start << "length is " << responseStr.length() << " and rest is " << rest << std::endl;
	// std::cout << "buf is " << buf << std::endl;
	// std::cout << "before start and rest is " << start << ", " << rest << "\n";
	if (rest > 65535)
	{
		// std::cout << "before\n";
		len = write(fd, &buf[start], 65535);
		// len = send(fd, &buf[start], 65536, 0);
		// std::cout << "after\n";
		start += len;
		rest -= len;
		// std::cout << "write " << len << " to client\n";
		// std::cout << "rest is " << rest << "\n";
		return ;
	}
	else
	{
		len = write(fd, &buf[start], rest);
		// printf("%s\n", &buf[start]);
	}
	// std::cout << "done write to client " << fd << "and rest is " << rest << "\n";
	// std::cout << "-------End responseStr-----------\n";
	// if (len == -1 || static_cast<unsigned long>(len) != responseStr.length())
	// std::cout << "-----------responseStr-----------\n";
	// if (responseStr.length() > 200)
	// 	std::cout << responseStr.substr(0, 200) << std::endl;
	// else
	// 	std::cout << responseStr << std::endl;
	// std::cout << "-----------responseStr-----------\n";
	if (len == -1)
	{
		std::cout << "write response error in fd" << fd << std::endl;
		EV_SET(&ev, fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
		EV_SET(&ev, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
		document.GetFdEvent().erase(fd);
		// std::cout << "response string is " << responseStr << std::endl;
		// std::cout << "end write error\n\n";
		// std::cout << "errno is " << errno << std::endl;
	}
	// write(fd, responseStr.c_str(), responseStr.length());
	// 헤더를 보고 서버연결 닫을지 말지 결정
	// 안닫으면 이벤트 writeable 빼주기
	std::cout << "complete write to client " << fd << "\n";
	if (response.GetHeader("Connection") == "close")
	{
		if (close(fd) < 0)
		{
			std::cout << "close error in connection close fd " << fd << "\n";
			// throw std::runtime_error("in sender connection close close error");
		}
		document.GetFdEvent().erase(fd);
		EV_SET(&ev, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
		// std::cout << "Connection closed fd " << fd << "\n";
	}
	document.RemoveResponse(fd);
	EV_SET(&ev, fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
	kevent(m_kq, &ev, 1, NULL, 0, NULL);
}