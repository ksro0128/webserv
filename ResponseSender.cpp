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
	int len;
	int& start = info->start;
	std::string& responseStr = response.GetResponse();
	const char *buf = responseStr.c_str();
	int& rest = info->rest;
	if (rest > 65535)
	{
		len = write(fd, &buf[start], 65535);
		start += len;
		rest -= len;
		return ;
	}
	else
		len = write(fd, &buf[start], rest);
	if (len == -1)
	{
		std::cout << "write response error in fd" << fd << std::endl;
		EV_SET(&ev, fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
		EV_SET(&ev, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
		document.GetFdEvent().erase(fd);
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